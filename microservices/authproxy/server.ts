import express from "express";
import fs from "fs";
import path from "path";
import {connectToMongoDatabase} from "./util/Database";
import cors from "cors";
import swaggerUi from "swagger-ui-express";
import {specs} from "./util/swagger";
import {printServerMessageStructure} from "./util/datatypes/MessagePackDataTypes";

const routesPath = path.join(__dirname, "routes");
export const app = express();
app.use(express.json());

app.use(cors({
    exposedHeaders: ["Authorization"]
}));

app.use('/api-docs', swaggerUi.serve, swaggerUi.setup(specs));

app.listen(8080, () => {
    console.log(`Server läuft auf http://localhost:8080`);
});

// Custom Function for loading HTTP dynamic routes in /routes/endpoint/method.ts
async function loadRoutes() {
    const folders = fs.readdirSync(routesPath, {withFileTypes: true})
        .filter(dirent => dirent.isDirectory() && !dirent.name.startsWith("ws_"))
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
    await printServerMessageStructure();
    await establishDataBaseConnection();
    //Start Ws Server After Database is connected
    await import("./routes/ws_calculate/ws_server");
    await initServerRoutes();
}

startServer().then(r => console.log("Server started!"));
