import {Request, Response} from "express";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/lobby/LobbyModel";
import {loadLobby, updateLobby} from "../../util/lobby/LobbyService";
import config from "config";

/**
 * @swagger
 * /lobbies:
 *   put:
 *     summary: Tritt einer bestehenden Lobby bei
 *     description: Fügt einen Benutzer zu einer Lobby hinzu, falls sie existiert und der Benutzer noch nicht drin ist.
 *     security:
 *       - bearerAuth: []
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             properties:
 *               lobbyName:
 *                 type: string
 *                 example: "Meine Lobby"
 *               lobbyPassword:
 *                 type: string
 *                 example: "geheimespasswort"
 *     responses:
 *       200:
 *         description: Erfolgreich der Lobby beigetreten
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Successfully Joined lobby!"
 *       400:
 *         description: Fehlerhafte Anfrage oder ungültige Lobby
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "lobby name is required!"
 *       409:
 *         description: Benutzer ist bereits in der Lobby
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "User Has Already Joined Lobby!"
 */
async function put(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "lobby name is required!"});
        return;
    }

    const lobbyPraefix: string = lobbyData.lobbyName.split("-")[0];
    let numberOfAllowedUser: number = config.get(lobbyPraefix) as number;
    if(!numberOfAllowedUser) {
        numberOfAllowedUser = config.get(lobbyPraefix) as number;
        console.info("Lobby name had no known prefix using default for now");
    }

    const userNameFromCreator = (request as any).userObject.username;
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

    if(alreadyExistendLobby.members.includes(userNameFromCreator)) {
        response.status(409).json({message: "User Has Already Joined Lobby!"});
        return
    }

    if(alreadyExistendLobby.members.length >= numberOfAllowedUser) {
        response.status(409).json({message: `Lobby is already full`});
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
export default [validateAuthorization, put];