import Mongoose, {Document} from 'mongoose';
import bcrypt from 'bcrypt';

export interface ILobby extends Document {
    lobbyName: string;
    lobbyPassword?: string;
    members: string[];
    open: boolean;
}

const lobbySchema = new Mongoose.Schema<ILobby>({
    lobbyName: {type: String, unique: true, required: true},
    lobbyPassword: {type: String, required: false, default: null},
    members: { type: [String], required: true },
    open: { type: Boolean, required: true }
});

export const LobbyModel = Mongoose.model<ILobby>('lobby', lobbySchema);
