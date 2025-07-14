FROM rust:latest

WORKDIR /app

COPY . .

RUN cargo build --release
ARG SERVICE_PORT
ENV SERVICE_PORT=${SERVICE_PORT}
CMD ["sh", "-c", "./target/release/calculation_unit $SERVICE_PORT"]

