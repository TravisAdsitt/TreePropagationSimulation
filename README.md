## Tree Propagation Simulation
This repo is aimed at an example C++ program that demonstrates tree propagation using SFML. It is paired with an article I wrote [here](https://wfale.net).

## Compilation
    mkdir build
    cd build
    cmake -G "MinGW Makefiles" ..
    make (or 'mingw32-make' on Windows with MinGW)

## Run
From the build directory:

    ./ObjectPropagation.exe

## Controls
When you run the program it may look like nothing is happening. In reality it is likely the view is very zoomed in. The controls for the view are as follows:

* **W** - Zoom out
* **S** - Zoom in
* **Left Arrow** - Move View Left
* **Right Arrow** - Move View Right
* **Up Arrow** - Move View Up
* **Down Arrow** - Move View Down
