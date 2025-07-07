import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {deleteLobby, searchLobbyOfMember} from "./LobbyService";
import {
    decodeToClientMessage,
    decodeToServerMessage, encodeClientMessage,
    encodeServerMessage
} from "../datatypes/MessagePackDataTypes";
import {
    CONTAINER_PREFIX,
    startCalculationUnit,
    startPongCalculationUnit,
    stopContainer
} from "../../docker-management/DockerManager";

export interface LobbyRegistryEntry {
    lobbyName: string;
    members: IUser[];
    memberSockets: WebSocket[];
    calculationSocket: WebSocket | null;
    containerInstanceNumber: number;
}

const registeredLobbys = new Set<LobbyRegistryEntry>();

export function isRegistered(lobbyName: string): LobbyRegistryEntry | null {
    return [...registeredLobbys].find(l => l.lobbyName === lobbyName) || null;
}

export function startCleanupScheduler() {
    setInterval(async () => {
        for (const lobby of registeredLobbys) {
            const allClosed = lobby.memberSockets.every(socket => socket.readyState === WebSocket.CLOSED);

            if (allClosed) {
                console.log(`Lobby ${lobby.lobbyName} wird geschlossen...`);
                if (lobby.calculationSocket) {
                    lobby.calculationSocket.close();
                    console.log(`Calculation-Socket für ${lobby.lobbyName} geschlossen.`);
                }
                await stopContainer(CONTAINER_PREFIX + lobby.containerInstanceNumber);
                registeredLobbys.delete(lobby);
                console.log(`Lobby ${lobby.lobbyName} aus Registry entfernt.`);
                console.log("Lobby DatabaseEntry removed successfully: ", await deleteLobby(lobby.lobbyName));
            }
        }
    }, 30000);
}

startCleanupScheduler();

export function addToRegister(lobby: LobbyRegistryEntry): boolean {
    if (isRegistered(lobby.lobbyName)) {
        return false;
    }
    registeredLobbys.add(lobby);
    return true
}

function findLowestPossibleContainerNumber(): number {
    let num = 0;
    let isUsed = false;
    while (true) {
        registeredLobbys.forEach((lobby) => {
            if (lobby.containerInstanceNumber === num) {
                isUsed = true;
            }
        })

        if (!isUsed) {
            break;
        }

        num++;
        isUsed = false;
    }
    return num;
}

function isWebsocketAlreadyUsedInAnotherLobby(newlyConnectedSocket: WebSocket): boolean {
    for (const lobby of registeredLobbys) {
        for (const socket of lobby.memberSockets) {
            if (socket === newlyConnectedSocket) {
                return true;
            }
        }
    }
    return false;
}

function findRegisteredLobbyOfWebsocket(messageSocket: WebSocket): LobbyRegistryEntry | null {
    for (const lobby of registeredLobbys) {
        for (const socket of lobby.memberSockets) {
            if (socket === messageSocket) {
                return lobby;
            }
        }
    }
    return null
}


export async function handleWebsocketMessage(ws: WebSocket, data: RawData, userData: IUser) {
    if (userData === undefined || userData.username === undefined) {
        console.error("User data could not be read properly! Please Check ducking tocking!");
        ws.close();
        return;
    }

    const uint8Array = data instanceof Buffer
        ? new Uint8Array(data)
        : new Uint8Array(data as ArrayBuffer);

    console.log("Received ClientMessage: ", uint8Array);
    const clientRequest: any = await decodeToClientMessage(uint8Array);
    if (!clientRequest) {
        console.error("Could not decode message");
        ws.close();
        return;
    }

    if (!isWebsocketAlreadyUsedInAnotherLobby(ws)) {
        // First Time Connection check if a new lobby needs to be registered
        const lobbyName: string = clientRequest[1][7];
        if (!lobbyName) {
            console.error("Asserted lobby connect message but did not found lobbyname: ", lobbyName);
            ws.close();
        }

        const userLobby: ILobby | null = await searchLobbyOfMember(lobbyName, userData.username);
        if (!userLobby) {
            console.error("User is not in a lobby!");
            ws.close();
            return;
        }

        let registerLobby: LobbyRegistryEntry | null = isRegistered(userLobby.lobbyName);
        if (registerLobby && !registerLobby.memberSockets.includes(ws)) {
            registerLobby.memberSockets.push(ws);
            registerLobby.members.push(userData);
        } else {
            const lowestContainerNumber = findLowestPossibleContainerNumber();
            registerLobby = {
                lobbyName: userLobby.lobbyName,
                members: [userData],
                memberSockets: [ws],
                calculationSocket: null,
                containerInstanceNumber: lowestContainerNumber
            };
            addToRegister(registerLobby);
            registerLobby.calculationSocket = await connectToCalculationServer(lowestContainerNumber, registerLobby);
        }
    }

    const registerLobby: LobbyRegistryEntry | null = findRegisteredLobbyOfWebsocket(ws);
    if(registerLobby === null) {
        console.error("Could not find register lobby!");
        ws.close();
        return;
    }

    const encoded = await encodeClientMessage(clientRequest);
    if (encoded === null) {
        console.error("Could not encode message");
        ws.close();
        return;
    }

    const calc_unit_socket: WebSocket | null = registerLobby.calculationSocket;
    if (!calc_unit_socket) {
        console.error("No calculation socket found! for lobby", registerLobby);
        ws.close();
        return;
    }

    console.log("Waiting for readyState before sending...");
    while (calc_unit_socket.readyState !== WebSocket.OPEN) {
        await new Promise(resolve => setTimeout(resolve, 500));
    }
    console.log("Calculation socket is open, sending data...");
    console.log("Sending To CalcUnit: ", String(encoded));
    calc_unit_socket.send(encoded);
}

async function connectToCalculationServer(containerNumber: number, lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    console.log("Connecting to calculation server...");
    if (lcomp.lobbyName.startsWith("3d-")) {
        await startCalculationUnit(containerNumber, 9000 + containerNumber);
    }else if (lcomp.lobbyName.startsWith("pong-")) {
        await startPongCalculationUnit(containerNumber, 9000 + containerNumber);
    }else {
        await startCalculationUnit(containerNumber, 9000 + containerNumber);
    }

    let socket;
    const lobbyPort = 9000 + containerNumber;
    await new Promise(resolve => setTimeout(resolve, 5000));
    socket = new WebSocket(`ws://calculation_unit_${containerNumber}:${lobbyPort}/msgpack`);

    socket.onopen = () => {
        console.log("WebSocket Connected to CalcUnit");
    };

    socket.on("message", async (msg) => {
        const uint8Array = msg instanceof Buffer
            ? new Uint8Array(msg)
            : new Uint8Array(msg as ArrayBuffer);

        const serverMessage: any = await decodeToServerMessage(uint8Array);
        let targetUsername: string | undefined = undefined;
        if (lcomp.lobbyName.startsWith("3d-")) {
            targetUsername = serverMessage[1][0];
        }else if (lcomp.lobbyName.startsWith("pong-")) {
            targetUsername = serverMessage[1];
        }else {
            targetUsername = serverMessage[1][0];
        }

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

            //console.log("sending: ", String(sendMessage));
            lcomp.memberSockets.forEach((ws: WebSocket) => {
                ws.send(sendMessage)
            })
            return;
        }

        const userIndex = lcomp.members.findIndex(member => member.username === targetUsername);
        if (userIndex === -1) {
            console.error("User not set in lobby!");
            return;
        }
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

    return socket;
}








