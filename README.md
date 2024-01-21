
# Visual A* Algorithm

Customizable A* algorithm created in C with real-time graphic display using to Raylib. 
- Press "R" to regenare the Start end End points.
- Press "Enter" to pause and play the algorithm.

By default the program pause the algorithm at the beginnig. 

Needs Raylib to work (https://www.raylib.com/).

All comments are in Italian because I didn't think of making the project public, but all the importants ones are translated down here.

## Screenshots

![App Screenshot](https://github.com/AndreSbro03/AStar/blob/main/screenshot.png)

## Installation

How to install my-project:

```bash
  chmod +x build.sh
  ./build.sh
  ./AStarAlgorithm
```
Alternately if you want to run the project as deamon:

```bash
  chmod +x build.sh
  ./build.sh
  ./AStarAlgorithm true
```

## Globals and variables

Globals:

- `S_DIMX` and `S_DIMY` are the dimensions of the screen.

- `C_DIM` is the cell size in pixels.

- `DST` more delicate constant, indicates the cost of crossing two consecutive cells. This value is added to the distance from the origin each time you continue a route. If it were 1 suboptimal paths but close to the origin would be developed instead of better paths but further from the origin. The higher a value, the more paths considered, therefore the solution will be more valid but the algorithm will be slower. Vice versa with values ​​closer to 0. Optimal values ​​are between 0.5 and 0.95. 

- `R_RANDOMNESS` 1/R_RANDOMNESS is the possibility that a cell is a solid block.

- `REG_POINT` if defined by pressing "R" you can randomly regenerate the position of Start and End.

- `PRINT_FINAL_ROUTE` if defined it prints to the terminal a list of all the nodes crossed by the final route.

- `RANDOM_SPAWN` if true Start and End are generated randomly, otherwise by default they are generated at opposite ends of the map.

- `STEP_DST` if true the distance function will be applied to a grid, if false the map will be like a graph and therefore the Pythagorean theorem will be used (slower method).

Variables:
- `pause` default as true it means that you have to start yourself the algorithm by pressing "Enter".

- `draw_final_route` if true (default) the program will draw the final route.

## Acknowledgements

 - [A* algorithm description]  https://en.wikipedia.org/wiki/A*_search_algorithm
