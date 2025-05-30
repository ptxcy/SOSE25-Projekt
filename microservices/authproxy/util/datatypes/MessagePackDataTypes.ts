import {decode, encode} from "@msgpack/msgpack";

export interface Coordinate {
    x: number;
    y: number;
    z: number;
}

export interface DummySetVelocity {
    id: string;
    position: Coordinate;
}

export interface SetClientFPS {
    id: string;
    fps: number;
}

export interface ClientRequest {
    set_client_fps?: SetClientFPS;
    spawn_dummy?: string;
    dummy_set_velocity?: DummySetVelocity;
    connect?: string;
}

export interface RequestInfo {
    client: { sent_time: number };
    authproxy: { sent_time: number };
    request_sync: { sent_time: number };
    calculation_unit: { sent_time: number };
}

export interface ClientMessage {
    request_info: RequestInfo;
    request_data: ClientRequest;
}

export interface ObjectData {
    target_user_id: string;
    game_objects: GameObjects;
}

export interface GameObjects {
    dummies: Record<string, DummyObject>;
}

export interface ServerMessage {
    request_info: RequestInfo;
    request_data: ObjectData;
}

export interface DummyObject {
    id: string;
    position: Coordinate;
    velocity: Coordinate;
}

export function printClientMessage(message: ClientMessage): void {
    console.log("ClientMessage:");
    console.log("Request Info:", message.request_info);
    console.log("Request Data:", JSON.stringify(message.request_data, null, 2));
}

export function printServerMessage(message: ServerMessage): void {
    console.log("ServerMessage:");
    console.log("Request Info:", message.request_info);
    console.log("Request Data:", JSON.stringify(message.request_data, null, 2));
}

export async function encodeClientMessage(object: ClientMessage): Promise<Uint8Array | null> {
    try {
        return encode(object);
    } catch (error) {
        console.error("Failed To Encode Message:", error);
        return null;
    }
}

export async function encodeServerMessage(object: ServerMessage): Promise<Uint8Array | null> {
    try {
        return encode(object);
    } catch (error) {
        console.error("Failed To Encode Message:", error);
        return null;
    }
}

export async function decodeToServerMessage(data: Uint8Array): Promise<ServerMessage | null> {
    try {
        return decode(data) as ServerMessage;
    } catch (error) {
        console.error("Failed To Decode Object because:", error);
        return null;
    }
}

export async function decodeToClientMessage(data: Uint8Array): Promise<ClientMessage | null> {
    try {
        return decode(data) as ClientMessage;
    } catch (error) {
        console.error("Failed To Decode Object because:", error);
        return null;
    }
}

