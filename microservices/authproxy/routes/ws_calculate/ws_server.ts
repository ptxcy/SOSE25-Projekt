import {WebSocketServer, WebSocket, RawData} from "ws";
import {
    AuthenticationResult,
    validateAuthentication
} from "../../util/AuthenticationService";
import {handleWebsocketMessage} from "../../util/lobby/LobbyManager";
const wss = new WebSocketServer({ port: 8083 });

wss.on("connection", async (ws: WebSocket, req) => {
    console.log("Somebody is trying to Handshake the websocket");
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

    ws.on("message", async (message: RawData) => {
        if (valid.userData) {
            // console.log("Try to handle message with user data: ", valid.userData.username); <<<<< debug
            await handleWebsocketMessage(ws, message, valid.userData);
        }else {
            console.error("User Data was invalid in sending this print is a duplicate only printed when a race condition happens!")
        }
    })

    ws.on("close", () => {
        console.log("WebSocket From Lobby Member has disconnected!");
    });
});

