import bcrypt from 'bcrypt';
import {IUser} from "./user/UserModel";
import {loadUser} from "./user/UserService";
import jwt, {JwtPayload} from "jsonwebtoken";

const PRIVATE_KEY = "PrivateDefaultKey"

export interface AuthenticationResult {
    userData: IUser | null;
    errorMessage?: string;
    success: boolean;
}

function createAuthResult(userData: IUser | null, success: boolean, errorMessage?: string): AuthenticationResult {
    return {
        userData,
        success,
        errorMessage
    };
}

export async function validateAuthentication(authHeaderValue: string): Promise<AuthenticationResult> {
    if (!authHeaderValue) {
        const error = "AuthHeader must be provided!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    let tokenRaw = "";
    if (authHeaderValue.startsWith("Bearer ")) {
        console.log("Bearer Token detected!");
        tokenRaw = authHeaderValue.substring(7);
    }else {
        console.error("Expected Bearer Token, but got: ", authHeaderValue);
    }

    console.log("Token: ", tokenRaw);
    const token = jwt.verify(tokenRaw, PRIVATE_KEY);
    if(!token || typeof token === "string") {
        const error = "AuthHeader was invalid!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    const username: string = token._doc.username;
    const userData: IUser | null = await loadUser(username);
    if (!userData) {
        const error = "user did not exist!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    return createAuthResult(userData, true);
}

export async function validateBasicAuthentication(authHeaderValue: string): Promise<AuthenticationResult> {
    if (!authHeaderValue) {
        const error = "AuthHeader must be provided!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    let base64decodedValue = "";
    try {
        base64decodedValue = Buffer.from(authHeaderValue.substring("Basic ".length), 'base64').toString('ascii');
    } catch (e) {
        const error = "Could not decode base64 value in authHeader!";
        console.error(error, e);
        return createAuthResult(null, false, error);
    }

    const [username, password] = base64decodedValue.split(':');
    if (!username || !password) {
        const error = "Invalid auth header format!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    const userData: IUser | null = await loadUser(username);
    if (!userData) {
        const error = "user did not exist!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    const validPassword = await validateUserPassword(password, userData.password);
    if (!validPassword) {
        const error = "Password doesn't match!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    return createAuthResult(userData, true);
}

export async function validateUserPassword(input: string, reference: string) {
    return bcrypt.compare(input, reference);
}

export async function generateJWTToken(userData: IUser): Promise<string> {
    return jwt.sign({...userData}, PRIVATE_KEY);
}

