import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/lobby/LobbyModel";
import {createLobby} from "../../util/lobby/LobbyService";

async function post(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "lobby name is required!"});
        return;
    }

    const userNameFromCreator = (request as any).userObject.userName;
    if (!userNameFromCreator) {
        response.status(400).json({message: "Malformed JWT Token"});
        return
    }

    let lobbyPassword: string | null | undefined = lobbyData.lobbyPassword;
    if (lobbyPassword === undefined) {
        lobbyPassword = null;
    }

    const tryToCreateLobby: ILobby | null = await createLobby(lobbyData.lobbyName, lobbyPassword, [userNameFromCreator]);
    if (tryToCreateLobby === null) {
        response.status(400).json({message: "Failed to create lobby!"});
        return;
    }

    response.status(200).json({message: "Successfully created lobby! Please start Web Socket Connection with lobby Info!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, validateAuthorization, post];