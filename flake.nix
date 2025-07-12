{
  inputs = {
    hyprland.url = "github:hyprwm/Hyprland";
    nixpkgs.follows = "hyprland/nixpkgs";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
      };
      mkClangShell = (pkgs.mkShell.override { stdenv = pkgs.clangStdenv; });
    in
    {
      packages.${system}.default = pkgs.callPackage ./package.nix { };

      devShells.${system}.default = mkClangShell {
        name = "show-my-osk";

        nativeBuildInputs = with pkgs; [
          meson
          ninja
          pkg-config
        ];

        buildInputs = with pkgs; [
          aquamarine
          hyprland
          hyprlang
          hyprutils
          hyprgraphics
          libdrm
          libinput
          libGL
          libxkbcommon
          pixman
          wayland
        ];
      };
    };
}
