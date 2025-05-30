import {IUser} from "../user/UserModel";
import {WebSocket, RawData} from "ws";
import {ILobby} from "./LobbyModel";
import {searchLobbyOfMember} from "./LobbyService";
import {
    ClientMessage,
    decodeToClientMessage,
    decodeToServerMessage,
    encodeClientMessage, encodeServerMessage, printServerMessage, ServerMessage
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

        const serverMessage: ServerMessage | null = await decodeToServerMessage(uint8Array);
        if(!serverMessage) {
            console.error("Could not decode server message");
            return;
        }

        const targetUsername: string | undefined = serverMessage?.request_data?.target_user_id;
        if (!targetUsername) {
            console.error("No target username found!");
            printServerMessage(serverMessage);
            return;
        }


        const userIndex = lcomp.members.findIndex(member => member.username === targetUsername);
        const userSocket = lcomp.memberSockets[userIndex];
        if(!userSocket) {
            console.error("No user socket!");
            return;
        }

        const sendMessage = await encodeServerMessage(serverMessage);
        if(!sendMessage) {
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








