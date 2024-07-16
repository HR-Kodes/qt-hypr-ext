{
  description = "A very basic flake";

  outputs = {
    self,
    nixpkgs,
  }: let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;
  in {
    packages.x86_64-linux.default = pkgs.libsForQt5.callPackage ./derivation_demo.nix {};
  };
}
