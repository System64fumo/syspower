{pkgs, ...}:
pkgs.stdenv.mkDerivation {
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
}
