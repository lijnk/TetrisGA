# TetrisGA
This program creates a neural network that plays tetris using a method called NeuroEvolution of Augmented Topologies (NEAT) as described in [this paper](http://nn.cs.utexas.edu/downloads/papers/stanley.ec02.pdf).

It includes general features such as saving and loading simulation files, pausing and is planned to have more features such as modifying and checking neurons as well.

The required headers and libraries are located in inc and lib respectively for cross-compiling for Windows, whereas Linux requires GLFW3 to be installed.

Uses [GLFW3](http://www.glfw.org/), stb\_image.h from [nothings.org](http://nothings.org), and a slightly modified version of BitStream Vera Sans Mono font (in order to fit things better).

The problem of solving Tetris is a difficult one and this program doesn't solve it, but is an attempt. In order to fix it, the input neurons as well as the fitness function will need to be reworked and I do not know of a method to do so, which means the project is at a halt until a solution is found.

