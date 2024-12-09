# Syspower
syspower is a basic power menu and shutdown screen written in gtkmm4<br>
![preview](https://github.com/System64fumo/syspower/blob/main/preview.gif "preview")

For now this only supports Hyprland.

# Configuration
syspower can be configured in 2 ways<br>
1: By changing config.hpp and recompiling (Suckless style)<br>
2: Using a config file (~/.config/sys64/power/config.conf)<br>
3: Using launch arguments<br>
```
arguments:
  -p	Set position (0 = top | 1 = right | 2 = bottom | 3 = left | 4 = center)
  -m	Set primary monitor (If you have multiple monitors)
  -v	Prints version info
  -t	Set revealer transition duration (0 = disabled)
```

# Nix

syspower can be installed on NixOS using the flake in this repository.

```nix
{
  inputs.syspower.url = "github:System64fumo/syspower";

  # in system config...
  environment.systemPackages = [inputs.syspower.packages.x86_64-linux.default];
}
```

It can also be configured with Home Manager:

```nix
{
  imports = [inputs.syspower.modules.homeManager.default];

  programs.syspower = {
    enable = true;
    settings  {
      position = "center";
      monitor = 0;
      revealer-transition-duration = 1000;
    };
    style = builtins.readFile ./style.css;
  }
}
```
