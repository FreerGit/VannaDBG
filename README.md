# VannaDBG
Vanna Volma Charm Banana

[![CI](https://github.com/freergit/VannaDBG/actions/workflows/ci.yml/badge.svg)][gh-ci]

[gh-ci]: https://github.com/freergit/evm-rs/actions/workflows/ci.yml

# Notes
deal w/ inlining

Bind the UI to the breakpoint, when a breakpoint is stepped over it should not display.

# Compile
Install glfw3 & opengl3:


sudo apt-get update

sudo apt-get install elfutils libdw-dev

sudo apt-get install libgl1-mesa-dev libglfw3-dev

# Assortment of todos that I will force myself to do eventually frfr
I really want to do a amalgamated build but can't get cimgui to play nice without cmake. Maybe one day, either way I will have to statically compile it to a executable when releasing.

File window

keybindings

Lazy loading of modules, loading everything at startup won't be a problem for my projects. I have no idea what would happen with say >100k LOC.