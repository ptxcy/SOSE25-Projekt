import {Request, Response} from "express";
import {validateAuthorization} from "../../middleware/is-jwt-token-valid";
import {listLobbies} from "../../util/lobby/LobbyService";

/**
 * @swagger
 * /lobbies:
 *   get:
 *     summary: Listet alle verfügbaren Lobbies auf
 *     description: Gibt eine Liste aller aktiven Lobbies zurück. Erfordert eine gültige JWT-Authentifizierung.
 *     security:
 *       - bearerAuth: []
 *     responses:
 *       200:
 *         description: Erfolgreiche Abfrage der Lobbies
 *         content:
 *           application/json:
 *             schema:
 *               type: array
 *               items:
 *                 type: object
 *                 properties:
 *                   id:
 *                     type: string
 *                     example: "abc123"
 *                   name:
 *                     type: string
 *                     example: "Lobby 1"
 *                   players:
 *                     type: number
 *                     example: 5
 *       401:
 *         description: Fehlende oder ungültige Authentifizierung
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Unauthorized"
 */
async function get(request: Request, response: Response) {
    response.status(200).json(await listLobbies());
}

//Export middleware and handler calls for dynamic routing
export default [validateAuthorization, get];