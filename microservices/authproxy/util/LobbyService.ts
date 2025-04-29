import { ILobby, LobbyModel } from "./LobbyModel";
import mongoose from "mongoose";

export async function loadLobby(lobbyName: string): Promise<ILobby | null> {
    return LobbyModel.findOne({ lobbyName: lobbyName });
}

export async function createLobby(lobbyName: string, lobbyPassword: string | null, members: string[]): Promise<ILobby | null> {
    try {
        const lobby = await LobbyModel.create({ lobbyName, lobbyPassword, members });
        return lobby;
    } catch (error) {
        console.error("Fehler beim Erstellen der Lobby:", error);
        return null;
    }
}

export async function deleteLobby(lobbyName: string): Promise<boolean> {
    const deleteResult: mongoose.DeleteResult = await LobbyModel.deleteOne({ lobbyName: lobbyName });
    return deleteResult.deletedCount === 1;
}

export async function listLobbies(): Promise<ILobby[]> {
    return LobbyModel.find();
}

export async function updateLobby(lobbyName: string, updateData: Partial<ILobby>): Promise<ILobby | null> {
    return LobbyModel.findOneAndUpdate({ lobbyName: lobbyName }, updateData, { new: true });
}
