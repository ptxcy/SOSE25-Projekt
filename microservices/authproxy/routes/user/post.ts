import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {IUser} from "../../util/UserModel";
import {createUser} from "../../util/UsersService";

async function post(request: Request, response: Response) {
   const userData: IUser = request.body;
   if (!userData.username || !userData.password) {
       response.status(400).json({message:"Username and password is required"});
       return;
   }

   await createUser(userData.username, userData.password);
   response.status(200).json({message:"Successfully created user"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, post];