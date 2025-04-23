import { Request, Response, NextFunction } from "express";

export function logRequest(request: Request, response: Response, next: NextFunction) {
    console.log("Request: ", request);
    next();
}