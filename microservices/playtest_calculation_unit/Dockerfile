FROM node:18

WORKDIR /app

COPY . /app

RUN npm install

ARG SERVICE_PORT
ENV PORT=9100
EXPOSE $PORT
CMD ["sh", "-c", "npm start"]

