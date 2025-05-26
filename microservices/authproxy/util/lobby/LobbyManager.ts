import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {searchLobbyOfMember} from "./LobbyService";
import {ClientMessage, decodeToObject, encodeObject} from "../datatypes/MessagePackDataTypes";

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
        registerLobby = {lobbyName: userLobby.lobbyName, members: [userData], memberSockets: [ws], calculationSocket: null};
        registerLobby.calculationSocket = await connectToCalcluationServer(registerLobby);
        addToRegister(registerLobby);
    }

    console.log("Decode");
    const uint8Array = data instanceof Buffer
        ? new Uint8Array(data)
        : new Uint8Array(data as ArrayBuffer);
    const clientRequest: ClientMessage | null = await decodeToObject(uint8Array);
    if (!clientRequest) {
        console.error("Could not decode message");
        ws.close();
        return;
    }

    console.log("Encode");
    const encoded = await encodeObject(clientRequest);
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

    // Warte in einer Schleife alle 0.5 Sekunden, bis der Socket bereit ist
    while (calc_unit_socket.readyState !== WebSocket.OPEN) {
        console.log("⏳ Waiting for WebSocket to open...");
        await new Promise(resolve => setTimeout(resolve, 500));
    }

    console.log("Calculation socket is open, sending data...");
    calc_unit_socket.send(encoded);
}

async function connectToCalcluationServer(lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    console.log("Connecting to calculation server...");
    const socket = new WebSocket(`ws://calculation_unit:8082/msgpack`);

    socket.onopen = () => {
        console.log("WebSocket erfolgreich verbunden mit der Berechnungseinheit!");
    };

    socket.on("message", (msg) => {
        console.log("Received message From Calculation", msg);
        lcomp.memberSockets.forEach(member => {
            member.send(msg);
        })
    })

    socket.on('close', () => {
        console.log('WebSocket closed');
    });

    console.log("WebSocket connected.");
    return socket;
}








