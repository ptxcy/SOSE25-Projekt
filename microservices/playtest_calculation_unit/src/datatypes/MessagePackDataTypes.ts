import {decode, encode} from "@msgpack/msgpack";
import {RawData} from "ws";

export type ServerMessage = [
    RequestInfo,
    ServerMessageObjects
];

export type RequestInfo = [
    number[],
    number[],
    number[],
    number[]
];

export type ServerMessageObjects = [
    string,
    ObjectContent
];

export type ObjectContent = [
    Record<string, DummyObject>,
    Planet[],
    Record<string, Player>,
    Record<number, SpaceShip>
];

export type SpaceShip = [
    number, // id
    string, // string
    number, // speed
    Coordinate, // velocity
    Coordinate, // position
    Coordinate  // target
]

export type Player = [
    string,
    number,
    CraftingMaterial
];

export type CraftingMaterial = [
    number
];

export type DummyObject = [
    string,
    number,
    string,
    Coordinate,
    Coordinate
];

export type Planet = [
    string, // Name
    Coordinate, // Position
    BuildingRegion[],
    number // size
];

export type Mine = [
    string,
    CraftingMaterial,
]

export type Factory = [
    string,
    CraftingMaterial,
]

export type BuildingRegion= [
    Coordinate,
    Factory[],
    Mine[],
    CraftingMaterial
]

export type Coordinate = [
    number,
    number,
    number
];

export type ClientData = [
    setClientFPS | null,
    string | null,
    dummySetVelocity | null,
    string | null,
    SetSpaceshipTarget | null
]

export type SetSpaceshipTarget = [
    number,
    Planet
]

export type dummySetVelocity = [
    number,
    Coordinate
]

export type setClientFPS = [
    number,
]

export type ClientMessage = [
    RequestInfo,
    ClientData,
    string,
]

export async function decodeToServerMessage(msg: RawData) {
    const uint8Array = msg instanceof Buffer
        ? new Uint8Array(msg)
        : new Uint8Array(msg as ArrayBuffer);

    return decode(uint8Array) as ServerMessage;
}

export async function encodeServerMessage(msg: ServerMessage) {
    return encode(msg);
}

export async function encodeClientMessage(msg: ClientMessage){
    return encode(msg);
}

export async function decodeToClientMessage(msg: RawData) {
    const uint8Array = msg instanceof Buffer
        ? new Uint8Array(msg)
        : new Uint8Array(msg as ArrayBuffer);

    return decode(uint8Array) as ClientMessage;
}

