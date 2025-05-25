import {IUser, UserModel} from "./UserModel";

export async function loadUser(username: string): Promise<IUser | null> {
    return UserModel.findOne({username: username});
}

export async function createUser(username: string, password: string): Promise<IUser | null> {
    try {
        return await UserModel.create({username, password});
    } catch (error) {
        console.error("Fehler beim Erstellen des Benutzers:", error);
        return null;
    }
}