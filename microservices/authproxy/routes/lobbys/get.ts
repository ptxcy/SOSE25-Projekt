import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {listLobbies} from "../../util/lobby/LobbyService";

async function get(request: Request, response: Response) {
    response.status(200).json(await listLobbies());
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, validateAuthorization, get];