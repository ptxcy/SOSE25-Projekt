import {Request, Response} from "express";
import {IUser} from "../../util/user/UserModel";
import {createUser} from "../../util/user/UserService";

async function post(request: Request, response: Response) {
    const userData: IUser = request.body;
    if (!userData.username || !userData.password) {
        response.status(400).json({message: "username and password is required!"});
        return;
    }

    const tryToCreateUser: IUser | null = await createUser(userData.username, userData.password);
    if (tryToCreateUser === null) {
        response.status(409).json({message: "User Already Exists Or Malformed Data!"});
    }
    response.status(200).json({message: "Successfully created user!"});
}

//Export middleware and handler calls for dynamic routing
export default [post];