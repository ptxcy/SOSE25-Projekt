import { Request, Response } from "express";
import { WebSocket } from "ws";
import {encode} from "@msgpack/msgpack";

function get(request: Request, response: Response) {
    const socket = new WebSocket(`ws://calculation_unit:8082/msgpack`, {
        headers: {
            Origin: 'http://authproxy:8080'
        }
    });

    socket.on('open', () => {
        console.log(`opened Socket`);
        const object = { author: "jonas", content: "hi" };
        let encoded: Uint8Array;
        encoded = encode(object);
        socket.send(encoded);
    });

    socket.on('message', (data) => {
        response.status(200).json({
            received: data.toString()
        });
        socket.close();
    });

    socket.on('error', (err) => {
        console.error("WebSocket error", err);
        if (!response.headersSent) {
            response.status(500).json({ error: "WebSocket connection failed." });
        }
    });

    socket.on('close', () => {
        console.log('WebSocket closed');
    });
}

// Export Middleware + Handler
export default [ get];
