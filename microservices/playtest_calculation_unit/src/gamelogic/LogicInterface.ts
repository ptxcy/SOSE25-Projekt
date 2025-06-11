import {setGameContext, getGameContext} from "../../server";
import {
    Coordinate,
    DummyObject,
    ObjectContent,
    Planet,
    ServerMessage,
    SpaceShip
} from "../datatypes/MessagePackDataTypes";

export function initEmptyGameContext(): ServerMessage {
    const defaultContext: ServerMessage = [
        [
            [
                0, // Client
            ],
            [
                0, //Authproxy
            ],
            [
                0, //rsync
            ],
            [
                0, //CalcUnit
            ]
        ],
        [
            "all",
            [
                {}, //DummyObjects
                [], //Planets
                {}, //Player
                {}  //SpaceShips
            ]
        ]
    ];
    return defaultContext;
}

function extractGameObjects(context: ServerMessage): ObjectContent | null {
    if (!context) return null;
    const content: ObjectContent = context[1][1];
    return content;
}

export function addPlanet(planet: Planet) {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    content[1].push(planet);
}

export function getPlanet(name: string): Planet | undefined {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    return content[1].find(planet => planet[0] === name);
}

export function movePlanet(name: string, moveTo: Coordinate) {
    const planet = getPlanet(name);
    if (!planet) return;
    planet[1] = moveTo;
}

export function deletePlanet(name: string) {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    const index = content[1].findIndex(planet => planet[0] === name);
    if (index > -1) {
        content[1].splice(index, 1);
    }
}

export function addSpaceShip(ship: SpaceShip) {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    const ships: Record<number, SpaceShip> = content[3];
    ships[ship[0]] = ship;
}

export function setVelocityForSpaceShip(id: number, moveTo: Coordinate) {
    const spaceShip: SpaceShip | undefined = getSpaceShip(id);
    if (!spaceShip) return;
    spaceShip[3] = moveTo;
}

export function moveSpaceShip(id: number, moveTo: Coordinate) {
    const spaceShip: SpaceShip | undefined = getSpaceShip(id);
    if (!spaceShip) return;
    spaceShip[4] = moveTo;
}

export function setTargetForSpaceShip(id: number, moveTo: Coordinate) {
    const spaceShip: SpaceShip | undefined = getSpaceShip(id);
    if (!spaceShip) return;
    spaceShip[5] = moveTo;
}

export function deleteSpaceShip(id: number) {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    const ships: Record<number, SpaceShip> = content[3];
    if (id in ships) {
        delete ships[id];
    }
}

export function getSpaceShip(id: number): SpaceShip | undefined {
    const context: ServerMessage | null = getGameContext();
    if (!context) return;
    const content = extractGameObjects(context);
    if (!content) return;
    const ships: Record<number, SpaceShip> = content[3];
    return ships[id];
}

function printGameContext() {
    const context: ServerMessage | null = getGameContext();
    if (!context) {
        console.log("Kein gültiger Kontext vorhanden.");
        return;
    }
    console.log(JSON.stringify(context, null, 2));
}

function testGameContext() {
    console.log("Initialer Kontext:");
    printGameContext();

    console.log("\nFüge einen Planeten hinzu:");
    addPlanet(["Mars", [0, 0, 0], [], 100]);
    printGameContext();

    console.log("\nBewege den Planeten:");
    movePlanet("Mars", [10, 10, 10]);
    printGameContext();

    console.log("\nLösche den Planeten:");
    deletePlanet("Mars");
    printGameContext();

    console.log("\nFüge ein Raumschiff hinzu:");
    addSpaceShip([1, "Explorer", 5, [0, 0, 0], [0, 0, 0], [10, 10, 10]]);
    printGameContext();

    console.log("\nBewege das Raumschiff:");
    moveSpaceShip(1, [5, 5, 5]);
    printGameContext();

    console.log("\nLösche das Raumschiff:");
    deleteSpaceShip(1);
    printGameContext();
}

testGameContext();
