import swaggerJsdoc from 'swagger-jsdoc';
import swaggerUi from 'swagger-ui-express';

const options: swaggerJsdoc.Options = {
    definition: {
        openapi: '3.0.0',
        info: {
            title: 'Authproxy API',
            version: '1.0.0',
        },
    },
    apis: ['./routes/**/*.ts'],
};

const specs = swaggerJsdoc(options);

export { specs, swaggerUi };
