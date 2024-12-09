{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = {flake-parts, ...} @ inputs:
    flake-parts.lib.mkFlake {inherit inputs;} rec {
      imports = [flake-parts.flakeModules.modules];

      systems = ["x86_64-linux"];

      flake.overlays.default = final: prev: {
        syspower = prev.callPackage ./package.nix {};
      };

      flake.modules.homeManager.default = {
        config,
        lib,
        pkgs,
        ...
      }: let
        inherit (lib) mkEnableOption mkPackageOption mkOption types mkIf;
        cfg = config.programs.syspower;
      in {
        options.programs.syspower = {
          enable = mkEnableOption "Syspower";
          package = mkPackageOption pkgs "syspower" {};
          settings = {
            position = mkOption {
              type = types.enum ["top" "right" "bottom" "left" "center"];
              default = "center";
              description = "Position of the menu on the screen";
            };
            monitor = mkOption {
              type = types.int;
              default = 0;
              description = "Monitor on which to show the menu";
            };
            transition-duration = mkOption {
              type = types.int;
              default = 1000;
              description = "Reveal transition duration in milliseconds";
            };
          };
          style = mkOption {
            type = types.str;
            default = builtins.readFile ./style.css;
            description = "CSS stylesheet to apply to the menu";
          };
        };

        config = mkIf cfg.enable {
          home.packages = [cfg.package];

          nixpkgs.overlays = [flake.overlays.default];

          home.file = {
            ".config/sys64/power/config.conf".text = ''
              [main]
              position=${builtins.toString (with cfg.settings;
                if position == "top"
                then 0
                else if position == "right"
                then 1
                else if position == "bottom"
                then 2
                else if position == "left"
                then 3
                else 4)}
              monitor=${builtins.toString cfg.settings.monitor}
              transition-duration=${builtins.toString cfg.settings.transition-duration}
            '';
            ".config/sys64/power/style.css".text = cfg.style;
          };
        };
      };

      perSystem = {
        pkgs,
        lib,
        ...
      }: rec {
        packages.default = pkgs.callPackage ./package.nix {};

        devShells.default = pkgs.mkShell {
          inputsFrom = [packages.default];
        };
      };
    };
}
