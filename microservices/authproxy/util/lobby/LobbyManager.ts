import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {searchLobbyOfMember} from "./LobbyService";
import {
    ClientMessage,
    decodeToClientMessage,
    encodeClientMessage, encodeServerMessage, printServerMessage, ServerMessage
} from "../datatypes/MessagePackDataTypes";
import {decode, ExtensionCodec} from "@msgpack/msgpack";

export interface LobbyRegistryEntry {
    lobbyName: string;
    members: IUser[];
    memberSockets: WebSocket[];
    calculationSocket: WebSocket | null;
}

const registeredLobbys = new Set<LobbyRegistryEntry>();

export function isRegistered(lobbyName: string): LobbyRegistryEntry | null {
    return [...registeredLobbys].find(l => l.lobbyName === lobbyName) || null;
}

export function startCleanupScheduler() {
    setInterval(() => {
        for (const lobby of registeredLobbys) {
            const allClosed = lobby.memberSockets.every(socket => socket.readyState === WebSocket.CLOSED);

            if (allClosed) {
                console.log(`Lobby ${lobby.lobbyName} wird geschlossen...`);
                if (lobby.calculationSocket) {
                    lobby.calculationSocket.close();
                    console.log(`Calculation-Socket für ${lobby.lobbyName} geschlossen.`);
                }

                registeredLobbys.delete(lobby);
                console.log(`Lobby ${lobby.lobbyName} aus Registry entfernt.`);
            }
        }
    }, 10000);
}
startCleanupScheduler();

export function addToRegister(lobby: LobbyRegistryEntry): boolean {
    if (isRegistered(lobby.lobbyName)) {
        return false;
    }
    registeredLobbys.add(lobby);
    return true
}

export async function handleWebsocketMessage(ws: WebSocket, data: RawData, userData: IUser) {
    console.log("Received message", data);
    if (userData === undefined || userData.username === undefined) {
        console.error("User data could not be read properly! Please Check ducking tocking!");
        ws.close();
        return;
    }

    console.log("Searching for Lobby of User: ", userData.username);
    const userLobby: ILobby | null = await searchLobbyOfMember(userData.username);
    if (!userLobby) {
        console.error("User is not in a lobby!");
        ws.close();
        return;
    }

    console.log("Handle Lobby Registry");
    let registerLobby: LobbyRegistryEntry | null = isRegistered(userLobby.lobbyName);
    if (registerLobby) {
        if (!registerLobby.memberSockets.includes(ws)) {
            registerLobby.memberSockets.push(ws);
        }
    } else {
        registerLobby = {
            lobbyName: userLobby.lobbyName,
            members: [userData],
            memberSockets: [ws],
            calculationSocket: null
        };
        registerLobby.calculationSocket = await connectToCalcluationServer(registerLobby);
        addToRegister(registerLobby);
    }

    console.log("Decode");
    const uint8Array = data instanceof Buffer
        ? new Uint8Array(data)
        : new Uint8Array(data as ArrayBuffer);

    console.log("recieved message pack", decode(uint8Array));
    const clientRequest: ClientMessage | null = await decodeToClientMessage(uint8Array);
    if (!clientRequest) {
        console.error("Could not decode message");
        ws.close();
        return;
    }

    console.log("Encode");
    const encoded = await encodeClientMessage(clientRequest);
    if (encoded === null) {
        console.error("Could not encode message");
        ws.close();
        return;
    }

    const calc_unit_socket: WebSocket | null = registerLobby.calculationSocket;
    if (!calc_unit_socket) {
        console.error("No calculation socket found!");
        ws.close();
        return;
    }

    console.log("Waiting for readyState before sending...");
    while (calc_unit_socket.readyState !== WebSocket.OPEN) {
        console.log("⏳ Waiting for WebSocket to open...");
        await new Promise(resolve => setTimeout(resolve, 500));
    }

    console.log("Calculation socket is open, sending data...");
    calc_unit_socket.send(encoded);
}

async function connectToCalcluationServer(lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    //TODO Close Socket If all user connections are closed
    console.log("Connecting to calculation server...");
    const socket = new WebSocket(`ws://calculation_unit:8082/msgpack`);

    socket.onopen = () => {
        console.log("WebSocket erfolgreich verbunden mit der Berechnungseinheit!");
    };

    socket.on("message", async (msg) => {
        const uint8Array = msg instanceof Buffer
            ? new Uint8Array(msg)
            : new Uint8Array(msg as ArrayBuffer);

        const rawMessage: any = decode(uint8Array);
        console.log("Received message From Calculation Unit", decode(uint8Array));
        const serverMessage: ServerMessage = {
            request_info: {
                client: { sent_time: 0 },
                authproxy: { sent_time: Date.now() },
                request_sync: { sent_time: 0 },
                calculation_unit: { sent_time: rawMessage[0][3] }
            },
            request_data: {
                target_user_id: rawMessage[1][0],
                game_objects: {
                    dummies: {}
                }
            }
        };

        const users = rawMessage[1][1][0];
        if (users && typeof users === 'object') {
            for (const userKey in users) {
                if (users.hasOwnProperty(userKey)) {
                    serverMessage.request_data.game_objects.dummies[userKey] = {
                        id: users[userKey][0],
                        position: { x: users[userKey][1][0], y: users[userKey][1][1], z: users[userKey][1][2] },
                        velocity: { x: users[userKey][2][0], y: users[userKey][2][1], z: users[userKey][2][2] }
                    };
                }
            }
        }


        if (!serverMessage) {
            console.error("Could not decode server message");
            return;
        }

        const targetUsername: string | undefined = serverMessage?.request_data?.target_user_id;
        if (!targetUsername) {
            console.error("No target username found!");
            return;
        }

        if (targetUsername === "all") {
            const sendMessage = await encodeServerMessage(serverMessage);
            if (!sendMessage) {
                console.error("ReEncoding Server Message Failed");
                return;
            }

            printServerMessage(serverMessage);
            lcomp.memberSockets.forEach((ws: WebSocket) => {
                ws.send(sendMessage)
            })
            return;
        }

        const userIndex = lcomp.members.findIndex(member => member.username === targetUsername);
        const userSocket = lcomp.memberSockets[userIndex];
        if (!userSocket) {
            console.error("No user socket!");
            return;
        }

        const sendMessage = await encodeServerMessage(serverMessage);
        if (!sendMessage) {
            console.error("ReEncoding Server Message Failed");
            return;
        }

        userSocket.send(sendMessage);
    })

    socket.on('close', () => {
        console.log('WebSocket closed');
    });

    console.log("WebSocket connected.");
    return socket;
}








