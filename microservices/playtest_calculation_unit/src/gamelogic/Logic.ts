import {
    addSpaceShip,
    addPlanet,
    movePlanet,
    moveSpaceShip,
    setTargetForSpaceShip,
    getPlanet,
    getSpaceShip,
    deletePlanet,
    deleteSpaceShip,
    setVelocityForSpaceShip, printGameContext
} from "./LogicInterface";

//********************************************
/* Planet Guide
 *
 * addPlanet(name: string, position: Coordinate, size: number): void
 *
 * movePlanet(name: string, moveTo: Coordinate): void
 *
 * deletePlanet(name: string): void
 *
 */

//********************************************
/* SpaceShip Guide
 *
 * addSpaceShip(id: number, speed: number, positon: Coordinate): void
 *
 * moveSpaceShip(moveTo: Coordinate): void
 *
 * setTargetForSpaceShip(targetTo: Coordinate): void
 *
 * setVelocityForSpaceShip(id: number, velocity: Coordinate)
 *
 * deleteSpaceShip(): void
 */

/*
 * HINTS
 * Coordinate is an array with 3 values = [x,y,z]
 * The First 8 planets have a fixed texture because of the techDemo the other have random textures
 */

//Create Your Logic
function delayedAction(action: Function, delay: number) {
    setTimeout(action, delay);
}

function exampleWorkFlow() {
    console.log("Initialer Kontext:");

    delayedAction(() => {
        console.log("\nFüge einen Planeten hinzu:");
        addPlanet("Mars", [1, 1, 0], 100);
        printGameContext();
    }, 5000);

    delayedAction(() => {
        console.log("\nBewege den Planeten:");
        movePlanet("Mars", [10, 10, 10]);
        printGameContext();
    }, 10000);

    delayedAction(() => {
        console.log("\nLösche den Planeten:");
        deletePlanet("Mars");
        printGameContext();
    }, 15000);

    delayedAction(() => {
        console.log("\nFüge ein Raumschiff hinzu:");
        addSpaceShip(1,1,[0,0,0]);
        printGameContext();
    }, 20000);

    delayedAction(() => {
        console.log("\nBewege das Raumschiff:");
        moveSpaceShip(1, [5, 5, 5]);
        printGameContext();
    }, 25000);

    delayedAction(() => {
        console.log("\nLösche das Raumschiff:");
        deleteSpaceShip(1);
        printGameContext();
    }, 30000);
}

exampleWorkFlow();