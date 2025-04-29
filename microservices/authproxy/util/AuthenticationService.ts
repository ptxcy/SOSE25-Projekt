import bcrypt from 'bcrypt';
import {IUser} from "./UserModel";
import {loadUser} from "./UserService";
import jwt, { JwtPayload } from "jsonwebtoken";

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

export async function validateBasicAuthentication(authHeaderValue: string): Promise<AuthenticationResult> {
    if (!authHeaderValue) {
        const error = "AuthHeader must be provided!";
        console.error(error);
        return createAuthResult(null, false, error);
    }

    let base64decodedValue = "";
    try {
        base64decodedValue = Buffer.from(authHeaderValue, 'base64').toString('ascii');
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
        const error = "User did not exist!";
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
    return jwt.sign({ ...userData }, "PrivateDefaultKey");
}

