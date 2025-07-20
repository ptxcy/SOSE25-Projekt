FROM node:18

WORKDIR /app

COPY . /app

WORKDIR /app/microservices/info-site

RUN npm install

EXPOSE 8080

CMD ["npm", "start"]
