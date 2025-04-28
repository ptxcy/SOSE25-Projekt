import {Request, Response, NextFunction} from "express";
import jwt, {JwtPayload} from "jsonwebtoken";
import {IUser} from "../util/UserModel";

const SECRET_KEY = "PrivateDefaultKey";

export function validateAuthorization(request: Request, response: Response, next: NextFunction) {
    const authorization: string | undefined = request.headers.authorization;

    if (!authorization) {
        response.status(401).json({message: "Authentication failed"});
        return;
    }

    if (!authorization.startsWith("Bearer ")) {
        response.status(401).json({message: "Expected Bearer Token"});
        return;
    }

    const jwtToken: string = authorization.substring("Bearer ".length);
    try {
        const token: string | JwtPayload = jwt.verify(jwtToken, SECRET_KEY);
        if (typeof token === "string") {
            response.status(401).json({ message: "Invalid Token Format" });
            return;
        }

        const userObject: IUser = token as JwtPayload as IUser;
        (request as any).userObject = userObject;
        next();
    } catch (error) {
        response.status(401).json({message: "Invalid Token"});
    }
}