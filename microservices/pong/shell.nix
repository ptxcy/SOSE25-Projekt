{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    # X11 libraries
    xorg.libX11
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    xorg.libXext
    xorg.libXfixes
    xorg.libXrender
    xorg.libXtst
    
    # Graphics and input
    mesa
    libGL
    libxkbcommon
    wayland
    wayland-protocols
    
    # Audio (macroquad often needs this)
    alsa-lib
    
    # Build tools
    pkg-config
    gcc
    fontconfig
    fontconfig.dev
    freetype
    freetype.dev
    openssl
    openssl.dev
  ];
  
  shellHook = ''
    export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [
      pkgs.xorg.libX11
      pkgs.xorg.libXrandr
      pkgs.xorg.libXinerama
      pkgs.xorg.libXcursor
      pkgs.xorg.libXi
      pkgs.xorg.libXext
      pkgs.xorg.libXfixes
      pkgs.xorg.libXrender
      pkgs.xorg.libXtst
      pkgs.mesa
      pkgs.libGL
      pkgs.libxkbcommon
      pkgs.wayland
      pkgs.alsa-lib
    ]}:$LD_LIBRARY_PATH"
    
    # Set up Wayland if needed
    export WAYLAND_DISPLAY="$WAYLAND_DISPLAY"
    export XDG_RUNTIME_DIR="$XDG_RUNTIME_DIR"
  '';
}
