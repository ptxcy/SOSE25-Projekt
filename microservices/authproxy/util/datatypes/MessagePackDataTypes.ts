import { decode, encode } from "@msgpack/msgpack";

export interface RequestInfo {
    lobbyName: string;
    client: { sent_time: number };
    authproxy: { sent_time: number };
    request_sync: { sent_time: number };
    calculation_unit: { sent_time: number };
}

export interface RequestData {
    calculation_type: string;
    calculated_data: object;
}

export interface CalculationRequest {
    request_info: RequestInfo;
    request_data: RequestData;
    calculation_result: object;
}

export async function encodeObject(object: CalculationRequest): Promise<Uint8Array> {
    return encode(object);
}

export async function decodeToObject(data: string): Promise<CalculationRequest> {
    const uint8Array = new Uint8Array(Buffer.from(data, "binary"));
    return decode(uint8Array) as CalculationRequest;
}

