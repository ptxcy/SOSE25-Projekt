import {WebSocketServer, WebSocket, RawData} from "ws";
import {createServer} from "http";
import {app} from "../../server";
import {
    AuthenticationResult,
    validateAuthentication,
    validateBasicAuthentication
} from "../../util/AuthenticationService";
import {handleWebsocketMessage} from "../../util/lobby/LobbyManager";

const server = createServer(app);
const wss = new WebSocketServer({server});

wss.on("connection", async (ws: WebSocket, req) => {
    if (req.url !== "/calculate") {
        console.error("Somebody tryed to connect to the wrong websocket route");
        ws.close();
        return;
    }

    console.log("WebSocket connected on /calculate");
    const authorization: string | undefined = req.headers["authorization"];
    if (!authorization) {
        console.error("Somebody tryed to connect but was not authenticated");
        ws.close();
        return;
    }

    const valid: AuthenticationResult = await validateAuthentication(authorization);
    if (!valid || !valid.success) {
        console.error("Somebody tryed to connect but was not authenticated");
        ws.close();
        return;
    }

    ws.on("message", async (message: RawData) => {
        if (valid.userData) {
            await handleWebsocketMessage(ws, message, valid.userData);
        }
    })

    ws.on("close", () => {
        console.log("WebSocket disconnected.");
    });
});

server.listen(8080, () => {
    console.log("Websocket server Listening on port 8080");
});

