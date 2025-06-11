import {WebSocketServer, WebSocket, RawData} from "ws";
import {ServerMessage} from "./src/datatypes/MessagePackDataTypes";
import {encode} from "@msgpack/msgpack";

let GAME_CONTEXT_MESSAGE: ServerMessage | null = null;
import "./src/gamelogic/LogicInterface";

export const FPS: number = 120;
const port: number = parseInt(process.env.PORT || "9000", 10);
const wss = new WebSocketServer({ port });
wss.on("connection", async (ws: WebSocket, req) => {
    ws.on("message", async (message: RawData) => {
        console.log("Received message", message);
        return;
    })

    ws.on("close", () => {
        console.log("Connection closed");
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

export function setGameContext(msg: ServerMessage) {
    GAME_CONTEXT_MESSAGE = msg;
}