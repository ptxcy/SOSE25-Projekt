import {decode, encode} from "@msgpack/msgpack";
import {RawData} from "ws";

export type ServerMessage = [
    RequestInfo,
    ServerMessageObjects
];

type RequestInfo = [
    number[],
    number[],
    number[],
    number[]
];

type ServerMessageObjects = [
    string,
    ObjectContent
];

type ObjectContent = [
    Record<string, DummyObject>,
    Planet[],
    Record<string, Player>,
    Record<number, SpaceShip>
];

type SpaceShip = [
    number,
    string,
    number,
    Coordinate,
    Coordinate,
    Coordinate,
    boolean,
    number | null,
]

type Player = [
    string,
    number,
    CraftingMaterial
];

type CraftingMaterial = [
    number
];

type DummyObject = [
    string,
    number,
    string,
    Coordinate,
    Coordinate
];

type Planet = [
    string,
    Coordinate,
    BuildingRegion[],
    number,
    Spacestation
];

type Spacestation = [
    number,
    number,
]

type Mine = [
    string,
    CraftingMaterial,
]

type Factory = [
    string,
    CraftingMaterial,
]

type BuildingRegion= [
    Coordinate,
    Factory[],
    Mine[],
    CraftingMaterial
]

type Coordinate = [
    number,
    number,
    number
];

type ClientData = [
    setClientFPS | null,
    string | null,
    dummySetVelocity | null,
    string | null,
    SetSpaceshipTarget | null
]

type SetSpaceshipTarget = [
    number,
    number
]

type dummySetVelocity = [
    number,
    Coordinate
]

type setClientFPS = [
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

