{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # C compiler and build tools
    gcc
    gnumake
    clang-tools
    ncurses
  ];
}
