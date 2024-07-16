{
  lib,
  stdenv,
  cmake,
  qt5,
  layer-shell-qt,
  qtstyleplugins,
  wrapQtAppsHook,
}:
stdenv.mkDerivation rec {
  pname = "";
  version = "";

  src = ./.;
  nativeBuildInputs = [cmake wrapQtAppsHook];
  buildInputs = [
    qt5.qtbase
    layer-shell-qt
    qtstyleplugins
  ];
}
