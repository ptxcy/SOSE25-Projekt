import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {IUser} from "../../util/UserModel";
import {createUser} from "../../util/UsersService";

async function post(request: Request, response: Response) {
   const userData: IUser = request.body;
   if (!userData.username || !userData.password) {
       response.status(400).json({message:"username and password is required!"});
       return;
   }

   const tryToCreateUser = await createUser(userData.username, userData.password);
   if(tryToCreateUser === null) {
       response.status(400).json({message:"Failed to create user!"});
   }

   response.status(200).json({message:"Successfully created user!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, post];