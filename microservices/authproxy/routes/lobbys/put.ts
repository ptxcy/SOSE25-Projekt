import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/lobby/LobbyModel";
import {loadLobby, updateLobby} from "../../util/lobby/LobbyService";

async function put(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "lobby name is required!"});
        return;
    }

    const userNameFromCreator = (request as any).userObject.userName;
    if (!userNameFromCreator) {
        response.status(400).json({message: "Malformed JWT Token"});
        return;
    }

    let lobbyPassword: string | null | undefined = lobbyData.lobbyPassword;
    if (lobbyPassword === undefined) {
        lobbyPassword = null;
    }

    const alreadyExistendLobby = await loadLobby(lobbyData.lobbyName);
    if (alreadyExistendLobby === null) {
        response.status(400).json({message: "lobby doesn't exist!"});
        return
    }

    alreadyExistendLobby.members.push(userNameFromCreator);
    const tryToUpdateLobby: ILobby | null = await updateLobby(lobbyData.lobbyName, alreadyExistendLobby);
    if (tryToUpdateLobby === null) {
        response.status(400).json({message: "Failed to update lobby!"});
        return;
    }

    response.status(200).json({message: "Successfully Joined lobby!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, validateAuthorization, put];