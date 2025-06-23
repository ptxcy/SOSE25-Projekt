{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    libGL
    glew
    freetype
    curl
    openssl
    gcc
    SDL2
    glm
    cpr
    boost
    msgpack-cxx
  ];
}

