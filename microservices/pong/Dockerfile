FROM rust:latest

WORKDIR /app

COPY . .
COPY ../calculation_unit ./calculation_unit
WORKDIR /app/pong
RUN cargo build --release
ARG SERVICE_PORT
ENV SERVICE_PORT=${SERVICE_PORT}

CMD ["sh", "-c", "./target/release/server $SERVICE_PORT"]
