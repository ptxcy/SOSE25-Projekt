import {Request, Response} from "express";
import {IUser} from "../../util/user/UserModel";
import {createUser} from "../../util/user/UserService";

/**
 * @swagger
 * /users:
 *   post:
 *     summary: Erstellt einen neuen Benutzer
 *     description: Erstellt einen neuen Benutzer mit einem Benutzernamen und Passwort.
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             properties:
 *               username:
 *                 type: string
 *                 example: "MaxMustermann"
 *               password:
 *                 type: string
 *                 example: "geheimespasswort"
 *     responses:
 *       200:
 *         description: Benutzer erfolgreich erstellt
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Successfully created user!"
 *       400:
 *         description: Fehlende oder ungültige Eingabe
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "username and password is required!"
 *       409:
 *         description: Benutzer existiert bereits oder fehlerhafte Daten
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "User Already Exists Or Malformed Data!"
 */
async function post(request: Request, response: Response) {
    const userData: IUser = request.body;
    if (!userData.username || !userData.password) {
        response.status(400).json({message: "username and password is required!"});
        return;
    }

    const tryToCreateUser: IUser | null = await createUser(userData.username, userData.password);
    if (tryToCreateUser === null) {
        response.status(409).json({message: "User Already Exists Or Malformed Data!"});
    }
    response.status(200).json({message: "Successfully created user!"});
}

//Export middleware and handler calls for dynamic routing
export default [post];