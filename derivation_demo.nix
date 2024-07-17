{
  lib,
  stdenv,
  pkgs,
}:
stdenv.mkDerivation rec {
  pname = "";
  version = "";

  src = ./.;
  nativeBuildInputs = with pkgs; [cmake kdePackages.wrapQtAppsHook];
  buildInputs = with pkgs; [
    qt6.qtbase
    kdePackages.layer-shell-qt
    kdePackages.qtwayland
    kdePackages.qttools
    # kdePackages.qtstyleplugins
  ];
}
