import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/LobbyModel";
import {createLobby} from "../../util/LobbyService";

async function post(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "Lobby name is required!"});
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
        response.status(400).json({message: "Failed to create Lobby!"});
        return;
    }

    response.status(200).json({message: "Successfully created Lobby! Please start Web Socket Connection with Lobby Info!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, validateAuthorization, post];