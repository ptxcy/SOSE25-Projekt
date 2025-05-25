import {Request, Response} from "express";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {ILobby} from "../../util/lobby/LobbyModel";
import {createLobby} from "../../util/lobby/LobbyService";

/**
 * @swagger
 * /lobbies:
 *   post:
 *     summary: Erstellt eine neue Lobby
 *     description: Erstellt eine Lobby mit einem optionalen Passwort und einem Ersteller. JWT-Authentifizierung ist erforderlich.
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
 *                 example: "Meine coole Lobby"
 *               lobbyPassword:
 *                 type: string
 *                 example: "geheimespasswort"
 *     responses:
 *       200:
 *         description: Lobby erfolgreich erstellt
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Successfully created lobby! Please start Web Socket Connection with lobby Info!"
 *       400:
 *         description: Fehlerhafte Anfrage (fehlender Name oder ungültiger JWT)
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "lobby name is required!"
 */
async function post(request: Request, response: Response) {
    const lobbyData: ILobby = request.body;
    if (!lobbyData.lobbyName) {
        response.status(400).json({message: "lobby name is required!"});
        return;
    }

    const userNameFromCreator = (request as any).userObject.username;
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
export default [validateAuthorization, post];