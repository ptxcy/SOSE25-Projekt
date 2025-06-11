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
    Coordinate,  // target
    boolean,
    number | null,
]

export type Player = [
    string,
    number,
    CraftingMaterial
];

export type DummyObject = [
    string,
    number,
    string,
    Coordinate,
    Coordinate
];

export type CraftingMaterial = [
    number
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

export type Mine = [
    string,
    CraftingMaterial,
]

export type Factory = [
    string,
    CraftingMaterial,
]

export type BuildingRegion = [
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

//---------------------------------------------------------------------------------------------------
/*
 * Client Message
 */

export type ClientMessage = [
    RequestInfo,
    ClientData,
    string,
]

export type ClientData = [
        setClientFPS | null,
        string | null, // spawn dummy
        dummySetVelocity | null,
        string | null, // connect
        SetSpaceshipTarget | null
]

export type SetSpaceshipTarget = [
    number,
    number
]

export type dummySetVelocity = [
    number,
    Coordinate
]

export type setClientFPS = [
    number,
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

export async function encodeClientMessage(msg: ClientMessage) {
    return encode(msg);
}

export async function decodeToClientMessage(msg: RawData) {
    const uint8Array = msg instanceof Buffer
        ? new Uint8Array(msg)
        : new Uint8Array(msg as ArrayBuffer);

    return decode(uint8Array) as ClientMessage;
}

