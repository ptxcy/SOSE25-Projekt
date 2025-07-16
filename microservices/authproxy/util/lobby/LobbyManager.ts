import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {deleteLobby, searchLobbyOfMember} from "./LobbyService";
import {
    decodeToClientMessage,
    decodeToServerMessage,
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

    if (!isWebsocketAlreadyUsedInAnotherLobby(ws)) {
        const clientRequest: any = await decodeToClientMessage(uint8Array);
        if (!clientRequest) {
            console.error("Could not decode message does not match Messagepack encoding!");
            ws.close();
            return;
        }

        let lobbyName: string = clientRequest[1][7];
        if (!lobbyName) {
            lobbyName = clientRequest[0][2];
        }

        if (!lobbyName) {
            console.error("Asserted lobby connect message but did not found lobbyname: ", lobbyName);
            ws.close();
        } else {
            console.log("Parsed Lobby name from first ever received Message: ", lobbyName);
        }

        const userLobby: ILobby | null = await searchLobbyOfMember(userData.username, lobbyName);
        if (!userLobby) {
            console.error("User is not in a databank lobby! lobby name / username", lobbyName, userData.username);
            ws.close();
            return;
        }

        let registerLobby: LobbyRegistryEntry | null = isRegistered(userLobby.lobbyName);
        if (registerLobby && !registerLobby.memberSockets.includes(ws)) {
            console.log("A Lobby instance already exits adding websocket to memberSockets!");
            registerLobby.memberSockets.push(ws);
            registerLobby.members.push(userData);
        } else {
            console.log("No Lobby exits until now creating registry!");
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
        console.info("registered websocket in register: ", registerLobby.lobbyName);
    }

    const registerLobby: LobbyRegistryEntry | null = findRegisteredLobbyOfWebsocket(ws);
    if (registerLobby === null) {
        console.error("Could not find register lobby!");
        ws.close();
        return;
    }

    const calc_unit_socket: WebSocket | null = registerLobby.calculationSocket;
    if (!calc_unit_socket) {
        console.error("No calculation socket found! for lobby", registerLobby);
        ws.close();
        return;
    }

    while (calc_unit_socket.readyState !== WebSocket.OPEN) {
        console.log("Waiting for readyState before sending...");
        await new Promise(resolve => setTimeout(resolve, 500));
    }

    //console.log("Sending To CalcUnit: ", String(encoded)); -<<<<<<<<<<<<<<< debug
    calc_unit_socket.send(uint8Array);
}

function extractOptionalString(raw: Uint8Array, index: number): string  | undefined {
    const tag = raw[index];

    // Null-Wert
    if (tag === 0xc0) return undefined;

    // fixstr (≤ 31 Bytes)
    if (tag >= 0xa0 && tag <= 0xbf) {
        const length = tag - 0xa0;
        const strBytes = raw.slice(index + 1, index + 1 + length);
        return new TextDecoder().decode(strBytes);
    }

    // str8 (≤ 255 Bytes)
    if (tag === 0xd9) {
        const length = raw[index + 1];
        const strBytes = raw.slice(index + 2, index + 2 + length);
        return new TextDecoder().decode(strBytes);
    }

    // str16 (≤ 65,535 Bytes)
    if (tag === 0xda) {
        const length = (raw[index + 1] << 8) | raw[index + 2];
        const strBytes = raw.slice(index + 3, index + 3 + length);
        return new TextDecoder().decode(strBytes);
    }

    // str32 (≤ ~4GB)
    if (tag === 0xdb) {
        const length =
            (raw[index + 1] << 24) |
            (raw[index + 2] << 16) |
            (raw[index + 3] << 8) |
            raw[index + 4];
        const strBytes = raw.slice(index + 5, index + 5 + length);
        return new TextDecoder().decode(strBytes);
    }

    return undefined;
}

async function connectToCalculationServer(containerNumber: number, lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    console.log("Connecting to calculation server...");
    if (lcomp.lobbyName.startsWith("3d-")) {
        console.info("Starting 3D Calculation Unit");
        await startCalculationUnit(containerNumber, 9000 + containerNumber);
    } else if (lcomp.lobbyName.startsWith("pong-")) {
        console.info("Starting PONG Calculation Unit");
        await startPongCalculationUnit(containerNumber, 9000 + containerNumber);
    } else {
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
        //const startTime = performance.now(); <- Time Tracking
        const uint8Array = msg instanceof Buffer
            ? new Uint8Array(msg)
            : new Uint8Array(msg as ArrayBuffer);

        let targetUsername: string | undefined = undefined;
        if (lcomp.lobbyName.startsWith("3d-")) {
            const serverMessage: any = await decodeToServerMessage(uint8Array);
            targetUsername = serverMessage[1][0];
        } else if (lcomp.lobbyName.startsWith("pong-")) {
            targetUsername = extractOptionalString(uint8Array,2);
        } else {
            const serverMessage: any = await decodeToServerMessage(uint8Array);
            targetUsername = serverMessage[1][0];
        }

        if (!targetUsername) {
            console.error("No target username found!");
            return;
        }

        if (targetUsername === "all") {
            lcomp.memberSockets.forEach((ws: WebSocket) => {
                ws.send(uint8Array)
            })
            //const endTime = performance.now();
            //console.log(`Broadcast dauerte: ${(endTime - startTime).toFixed(2)} ms`);
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

        userSocket.send(uint8Array);
        //const endTime = performance.now();
        //console.log(`Broadcast dauerte: ${(endTime - startTime).toFixed(2)} ms`);
    })

    socket.on('close', () => {
        console.log('WebSocket closed');
    });

    return socket;
}








