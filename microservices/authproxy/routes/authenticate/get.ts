import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";
import {AuthenticationResult, generateJWTToken, validateBasicAuthentication} from "../../util/AuthenticationService";

async function get(request: Request, response: Response) {
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
    
    response.status(200)
        .setHeader("Authenticate", await generateJWTToken(authenticationResult.userData))
        .json({message: "Authentication was successful!"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, get];