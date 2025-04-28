import * as mongoose from "mongoose";
import {loadUser, createUser} from "./UsersService";

const defaultAdminPassword = "admin";

export async function connectToMongoDatabase() {
    mongoose.connect("mongodb://mongodb:27017/strategy-game")
        .then(() => console.log("Connected to MongoDB"))
        .catch(err => console.error("Failed to connect:", err));

    const adminUser = await loadUser("admin");
    if (adminUser !== null) {
        console.log("Admin user already exists");
        return;
    }

    const tryCreatingAdmin = await createUser("admin", defaultAdminPassword);
    if (tryCreatingAdmin === null) {
        console.log("Failed to create admin user");
    }
}