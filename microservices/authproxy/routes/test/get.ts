import {Request, Response} from "express";
import {logRequest} from "../../middleware/request-logger";

function get(request: Request, response: Response) {
    response.json({message: "Request was successfully registered"});
}

//Export middleware and handler calls for dynamic routing
export default [logRequest, get];