import {IUser, UserModel} from "./UserModel";
import Mongoose from "mongoose";

export async function loadUser(username: string): Promise<IUser | null> {
    return UserModel.findOne({username: username});
}

export async function createUser(username: string, password: string): Promise<IUser> {
    return UserModel.create({username, password});
}

async function deleteUser(username: string): Promise<boolean> {
    const deleteResult: Mongoose.DeleteResult = await UserModel.deleteOne({username: username});
    return deleteResult.deletedCount === 1;
}

async function listUsers(): Promise<IUser[]> {
    return UserModel.find();
}

async function updateUser(username: string, password: string): Promise<void> {
   const updateResult = UserModel.findOneAndUpdate({username: username, password: password})
}