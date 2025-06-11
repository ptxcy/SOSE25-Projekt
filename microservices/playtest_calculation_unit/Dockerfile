FROM node:18

WORKDIR /app

COPY . /app

RUN npm install

ARG SERVICE_PORT
ENV PORT=$SERVICE_PORT
EXPOSE $PORT
CMD ["npm", "start"]
