import {IUser} from "../user/UserModel";
import {WebSocket, RawData, OPEN} from "ws";
import {CalculationRequest, decodeToObject} from "../datatypes/MessagePackDataTypes";
import {loadLobby} from "./LobbyService";
import {ILobby} from "./LobbyModel";
import {encode} from "@msgpack/msgpack";

export enum LobbyState {
    OPEN,
    CLOSED
}

export interface LobbyComponent {
    lobbyName: string;
    lobbyPassword: string | undefined;
    lobbyStatus: LobbyState;
    lobbyInfo: ILobby

    members: IUser[];
    memberSockets: WebSocket[];
    calculationSocket: WebSocket | null;
}

const registeredLobbys: LobbyComponent[] = [];

export function addLobby(lobby: LobbyComponent) {
    registeredLobbys.push(lobby);
}

export async function handleWebsocketMessage(ws: WebSocket, data: RawData, userData: IUser) {
    const calc_request: CalculationRequest = await decodeToObject(data.toString());
    let lobbyComponent: LobbyComponent | undefined = registeredLobbys.find(lc => lc.lobbyName === calc_request.request_info.lobbyName);
    if (lobbyComponent) {
        if (!lobbyComponent.members.includes(userData) && lobbyComponent.lobbyInfo.members.includes(userData.username)) {
            lobbyComponent.members.push(userData);
            lobbyComponent.memberSockets.push(ws);
        }
    }

    if (!lobbyComponent) {
        lobbyComponent = await validateLobby(ws, calc_request, userData);
        if (!lobbyComponent) {
            console.error("Malformed Websocket Request from Authorized User!");
            ws.close();
            return;
        }
        addLobby(lobbyComponent);
    }

    if(lobbyComponent.calculationSocket !== null) {
        lobbyComponent.calculationSocket.send(data);
    }
}

export async function validateLobby(ws: WebSocket, calc_request: CalculationRequest, userData: IUser): Promise<LobbyComponent | undefined> {
    const lobbyName = calc_request.request_info.lobbyName;
    const databaseLoadedLobby = await loadLobby(lobbyName);
    if (databaseLoadedLobby === null) {
        return undefined;
    }

    if (!databaseLoadedLobby.members.includes(userData.username)) {
        return undefined;
    }

    const lcompo: LobbyComponent = {
        lobbyName: databaseLoadedLobby.lobbyName,
        lobbyPassword: databaseLoadedLobby.lobbyPassword,
        lobbyStatus: OPEN,
        lobbyInfo: databaseLoadedLobby,
        members: [userData],
        memberSockets: [ws],
        calculationSocket: null,
    };

    lcompo.calculationSocket = await connectToCalcluationServer(lcompo);
    return lcompo;
}

async function connectToCalcluationServer(lcomp: LobbyComponent): Promise<WebSocket> {
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







