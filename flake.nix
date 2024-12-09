{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = {flake-parts, ...} @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} {
      systems = ["x86_64-linux"];
      perSystem = {
        pkgs,
        lib,
        ...
      }: let
      in rec {
        packages.default = pkgs.stdenv.mkDerivation {
          name = "syspower";
          src = ./.;
          buildInputs = with pkgs; [gtkmm4 gtk4-layer-shell];
          nativeBuildInputs = with pkgs; [
            pkg-config
            makeWrapper
          ];

          buildPhase = ''
            make
          '';

          installPhase = ''
            install -D -t $out/bin syspower
            install -D -t $out/lib libsyspower.so
            install -D -t $out/share/sys64/power config.conf style.css
            wrapProgram $out/bin/syspower --prefix LD_LIBRARY_PATH : $out/lib
          '';
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [packages.default];
        };
      };
    };
}
