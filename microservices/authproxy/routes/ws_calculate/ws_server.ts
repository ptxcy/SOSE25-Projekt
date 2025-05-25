import {WebSocketServer, WebSocket, RawData} from "ws";
import {createServer} from "http";
import {app} from "../../server";
import {
    AuthenticationResult,
    validateAuthentication
} from "../../util/AuthenticationService";
import {handleWebsocketMessage} from "../../util/lobby/LobbyManager";

const server = createServer(app);
const wss = new WebSocketServer({server});

wss.on("connection", async (ws: WebSocket, req) => {
    console.log("Somebody is trying to connect to the websocket");
    const path = req.url?.split("?")[0];
    if (path !== "/calculate") {
        console.error("Somebody tried to connect to the wrong websocket route");
        ws.close();
        return;
    }

    const queryParams = req.url?.split("?")[1];
    const params = new URLSearchParams(queryParams);
    let authorization: string | null = params.get("authToken");
    if (!authorization) {
        console.error("Somebody tried to connect but was not authenticated");
        ws.close();
        return;
    }

    const valid: AuthenticationResult = await validateAuthentication(authorization);
    if (!valid || !valid.success) {
        console.error("Somebody tryed to connect but was not authenticated");
        ws.close();
        return;
    }

    console.log("✅ Authentifizierung erfolgreich!");
    ws.on("message", async (message: RawData) => {
        console.error("Received message", message);
        if (valid.userData) {
            await handleWebsocketMessage(ws, message, valid.userData);
        }
    })

    ws.on("close", () => {
        console.log("WebSocket disconnected.");
    });
});

server.listen(8083, () => {
    console.log("Websocket server Listening on port 8083");
});

