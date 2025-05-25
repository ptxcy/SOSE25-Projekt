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
        registerLobby = {lobbyName: userLobby.lobbyName, members: [userData], memberSockets: [ws], calculationSocket: null};
        registerLobby.calculationSocket = await connectToCalcluationServer(registerLobby);
        addToRegister(registerLobby);
    }

    const clientRequest: ClientMessage | null = await decodeToObject(data.toString());
    if (!clientRequest) {
        return;
    }

    const encoded = await encodeObject(clientRequest);
    if (encoded === null) {
        return;
    }

    const calc_unit_socket: WebSocket | null = registerLobby.calculationSocket;
    registerLobby.memberSockets.forEach(member => {
        if (!calc_unit_socket) {
            console.error("No calculation socket found!");
            return;
        }
        member.send(encoded);
    })
}

async function connectToCalcluationServer(lcomp: LobbyRegistryEntry): Promise<WebSocket> {
    const socket = new WebSocket(`ws://calculation_unit:8082/msgpack`, {
        headers: {
            Origin: 'http://authproxy:8080'
        }
    });

    socket.on("message", (msg) => {
        lcomp.memberSockets.forEach(member => {
            member.send(msg);
        })
    })

    socket.on('close', () => {
        console.log('WebSocket closed');
    });

    return socket;
}







