import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/LobbyModel";
import {createLobby, loadLobby, updateLobby} from "../../util/LobbyService";

async function put(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "Lobby name is required!"});
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
        response.status(400).json({message: "Lobby doesn't exist!"});
        return
    }

    alreadyExistendLobby.members.push(userNameFromCreator);
    const tryToUpdateLobby: ILobby | null = await updateLobby(lobbyData.lobbyName, alreadyExistendLobby);
    if (tryToUpdateLobby === null) {
        response.status(400).json({message: "Failed to update Lobby!"});
        return;
    }

    response.status(200).json({message: "Successfully Joined Lobby!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, validateAuthorization, put];