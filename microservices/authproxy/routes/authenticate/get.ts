import {Request, Response} from "express";
import {AuthenticationResult, generateJWTToken, validateBasicAuthentication} from "../../util/AuthenticationService";

/**
 * @swagger
 * /authenticate:
 *   get:
 *     summary: Authentifiziert einen Benutzer über Basic Authentication
 *     description: Überprüft die Anmeldedaten und gibt ein JWT zurück, falls die Authentifizierung erfolgreich ist.
 *     parameters:
 *       - in: header
 *         name: Authorization
 *         required: true
 *         schema:
 *           type: string
 *         description: "Basic Authentication Header (Base64-kodierter Benutzername:Passwort)"
 *     responses:
 *       200:
 *         description: Erfolgreiche Authentifizierung
 *         headers:
 *           Authorization:
 *             schema:
 *               type: string
 *             description: "Bearer Token zur weiteren Authentifizierung"
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Authentication was successful!"
 *       400:
 *         description: Fehlerhafte Anfrage oder ungültige Anmeldedaten
 *         content:
 *           application/json:
 *             schema:
 *               type: object
 *               properties:
 *                 message:
 *                   type: string
 *                   example: "Authorization header required!"
 */
async function get(request: Request, response: Response) {
    console.log("Somebody tried to authenticate");
    let authHeaderValue = request.headers?.authorization;
    if (!authHeaderValue) {
        response.status(400).json({message: 'Authorization header required!'});
        return;
    }

    const authenticationResult: AuthenticationResult = await validateBasicAuthentication(authHeaderValue);
    if (!authenticationResult.success || authenticationResult.userData === null) {
        response.status(400).json({message: authenticationResult.errorMessage});
        return;
    }

    console.log("Authentication was successful!");
    response.status(200)
        .setHeader("Authorization", "Bearer " + await generateJWTToken(authenticationResult.userData))
        .json({message: "Authentication was successful!"});
}

//Export middleware and handler calls for dynamic routing
export default [get];