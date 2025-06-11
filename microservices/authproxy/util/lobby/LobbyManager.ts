import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {searchLobbyOfMember} from "./LobbyService";
import {
    ClientMessage,
    decodeToClientMessage,
    decodeToServerMessage, encodeClientMessage,
    encodeServerMessage,
    ServerMessage
} from "../datatypes/MessagePackDataTypes";

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
    if (userData === undefined || userData.username === undefined) {
        console.error("User data could not be read properly! Please Check ducking tocking!");
        ws.close();
        return;
    }

    const userLobby: ILobby | null = await searchLobbyOfMember(userData.username);
    if (!userLobby) {
        console.error("User is not in a lobby!");
        ws.close();
        return;
    }

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
        addToRegister(registerLobby);
        registerLobby.calculationSocket = await connectToCalculationServer(registerLobby);
    }

    const uint8Array = data instanceof Buffer
        ? new Uint8Array(data)
        : new Uint8Array(data as ArrayBuffer);

    console.log("Received ClientMessage: ", uint8Array);
    const clientRequest: ClientMessage = await decodeToClientMessage(uint8Array);
    if (!clientRequest) {
        console.error("Could not decode message");
        ws.close();
        return;
    }

    clientRequest[0][1][0] = Date.now();
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
    console.log("Sending To CalcUnit: ", String(encoded));
    calc_unit_socket.send(encoded);
}

async function connectToCalculationServer(lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    //TODO Close Socket If all user connections are closed
    console.log("Connecting to calculation server...");

    let socket;
    if (lcomp.lobbyName.startsWith("playtest_")) {
        let playTesterLobbyCount = 0
        registeredLobbys.forEach((lobby) => {
            if (lobby.lobbyName.startsWith("playtest_")) {
                playTesterLobbyCount++;
            }
        });
        const playtestLobbyPort = 9099 + playTesterLobbyCount;
        socket = new WebSocket(`ws://calculation_unit_${playTesterLobbyCount}:${playtestLobbyPort}`);
    } else {
        let lobbyCount = 0
        registeredLobbys.forEach((lobby) => {
            if (!lobby.lobbyName.startsWith("playtest_")) {
                lobbyCount++;
            }
        });
        const lobbyPort = 9089 + lobbyCount;
        socket = new WebSocket(`ws://calculation_unit_${lobbyCount}:${lobbyPort}/msgpack`);
    }

    socket.onopen = () => {
        console.log("WebSocket Connected to CalcUnit");
    };

    socket.on("message", async (msg) => {
        const uint8Array = msg instanceof Buffer
            ? new Uint8Array(msg)
            : new Uint8Array(msg as ArrayBuffer);

        const serverMessage: ServerMessage = await decodeToServerMessage(uint8Array);
        const targetUsername: string | undefined = serverMessage[1][0];
        if (!targetUsername) {
            console.error("No target username found!");
            return;
        }

        //Update Time Stamp
        serverMessage[0][1][0] = Date.now();
        if (targetUsername === "all") {
            const sendMessage = await encodeServerMessage(serverMessage);
            if (!sendMessage) {
                console.error("ReEncoding Server Message Failed");
                return;
            }

            console.log("sending: ", String(sendMessage));
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

        console.log("sending: ", String(sendMessage));
        userSocket.send(sendMessage);
    })

    socket.on('close', () => {
        console.log('WebSocket closed');
    });

    console.log("WebSocket connected.");
    return socket;
}








