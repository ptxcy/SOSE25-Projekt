import {WebSocketServer, WebSocket, RawData} from "ws";
import {decodeToClientMessage, ServerMessage} from "./src/datatypes/MessagePackDataTypes";
import {decode, encode} from "@msgpack/msgpack";
import "./src/gamelogic/LogicInterface";
import {initEmptyGameContext, testGameContext} from "./src/gamelogic/LogicInterface";

interface CONTEXT {
    sockets: WebSocket[];
    players: string[],
    context: ServerMessage
}

const GAME_CONTEXT: CONTEXT = {
    sockets: [],
    players: [],
    context: initEmptyGameContext(),
}

export const FPS: number = 60;
const port: number = parseInt(process.env.PORT || "9000", 10);
const wss = new WebSocketServer({port});
wss.on("connection", async (ws: WebSocket, req) => {
    ws.on("message", async (message: RawData) => {
        console.log("Received message", message);
        // Should be a Client Message
        const clientMessage = await decodeToClientMessage(message);
        const connectContent: string | null = clientMessage[1][3];
        if (connectContent === null) {
            return;
        }

        GAME_CONTEXT.players.push(connectContent);
        GAME_CONTEXT.sockets.push(ws);
        return;
    })

    ws.on("close", () => {
        const socketIndexOfUser = GAME_CONTEXT.sockets.findIndex(ws => ws === ws);
        GAME_CONTEXT.sockets.splice(socketIndexOfUser, 1);
        GAME_CONTEXT.players.splice(socketIndexOfUser, 1);
        console.log("Connection closed");
        return;
    });

    sendPeriodicRenderMessages(ws);
});

async function sendPeriodicRenderMessages(webSocket: WebSocket) {
    setInterval(() => {
        GAME_CONTEXT.players.forEach(player => {
            GAME_CONTEXT.context[1][0] = player
            webSocket.send(encode(GAME_CONTEXT.context));
        })
    }, (1000 / FPS))
}

export function getGameContext(): ServerMessage | null {
    return GAME_CONTEXT.context;
}

export function setGameContext(msg: ServerMessage) {
    GAME_CONTEXT.context = msg;
}

testGameContext();