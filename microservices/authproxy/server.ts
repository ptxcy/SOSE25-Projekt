import express from "express";
import fs from "fs";
import path from "path";
import {connectToMongoDatabase} from "./util/Database";

const port = 8080;
const routesPath = path.join(__dirname, "routes");
const app = express();

app.use(express.json());

app.listen(port, () => {
    console.log(`Server läuft auf http://localhost:${port}`);
});

// Custom Function for loading dynamic routes in /routes/endpoint/method.ts
async function loadRoutes() {
    const folders = fs.readdirSync(routesPath, { withFileTypes: true })
        .filter(dirent => dirent.isDirectory())
        .map(dirent => dirent.name);

    for (const folder of folders) {
        const routePath = path.join(routesPath, folder);
        const files = fs.readdirSync(routePath);

        for (const file of files) {
            const method = file.split(".")[0].toLowerCase();
            const supportedMethods = ["get", "post", "put", "delete", "patch"];
            if (!supportedMethods.includes(method)) continue;
            const filePath = path.join(routePath, file);
            const handlerModule = await import(filePath);
            const handler = handlerModule.default;

            if (!handler) continue;
            (app as any)[method](`/${folder}`, ...(Array.isArray(handler) ? handler : [handler]));
            console.log(`Registered route: [${method.toUpperCase()}] /${folder}`);
        }
    }
}

async function establishDataBaseConnection() {
    await connectToMongoDatabase();
}

async function initServerRoutes() {
    await loadRoutes();
}

async function startServer() {
    await establishDataBaseConnection();
    await initServerRoutes();
}

startServer().then(r => console.log("Server started!"));
