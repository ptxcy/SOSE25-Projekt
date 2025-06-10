import {WebSocketServer, WebSocket, RawData} from "ws";
import {ServerMessage} from "./src/datatypes/MessagePackDataTypes";
import {encode} from "@msgpack/msgpack";
let GAME_CONTEXT_MESSAGE: ServerMessage | null = null;
import "./src/gamelogic/LogicInterface";

export const FPS: number = 120;
const wss = new WebSocketServer({port: 9000});
wss.on("connection", async (ws: WebSocket, req) => {
    ws.on("message", async (message: RawData) => {
        return;
    })

    ws.on("close", () => {
        return;
    });
    sendPeriodicRenderMessages(ws);
});

async function sendPeriodicRenderMessages(webSocket: WebSocket) {
    setInterval(() => {
        if (GAME_CONTEXT_MESSAGE !== null) {
            webSocket.send(encode(GAME_CONTEXT_MESSAGE));
        }
    }, (1000 / FPS))
}

export function getGameContext(): ServerMessage | null {
    return GAME_CONTEXT_MESSAGE;
}

export function setGameContext(msg: ServerMessage){
    GAME_CONTEXT_MESSAGE = msg;
}