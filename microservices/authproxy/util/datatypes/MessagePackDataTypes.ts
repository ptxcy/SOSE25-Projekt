import { decode, encode } from "@msgpack/msgpack";

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

export async function encodeObject(object: ClientMessage): Promise<Uint8Array | null> {
    try {
        return encode(object);
    } catch (error) {
        console.error("Failed To Encode Message:", error);
        return null;
    }
}

export async function decodeToObject(data: Uint8Array): Promise<ClientMessage | null> {
    try {
        return decode(data) as ClientMessage;
    } catch (error) {
        console.error("Failed To Decode Object because:", error);
        return null;
    }
}

