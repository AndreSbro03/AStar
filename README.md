
# Visual A* Algorithm 🚀🔍

A customizable **A\*** algorithm written in **C** with real-time graphical display using **Raylib**. This project allows you to visualize the A* search algorithm and interact with the grid by adding or removing obstacles, regenerating start and end points, and controlling the algorithm's execution.

### Key Features
- 🕹 **Interactive Controls**:
  - Press **"R"** to regenerate the start and end points.
  - Press **"Enter"** to pause or resume the algorithm.
  - **Add obstacles** with the left mouse button.
  - **Remove obstacles** with the right mouse button.
- ⏸ **Paused by Default**: The algorithm starts in a paused state, allowing you to set up the environment before starting.
- 💻 **Real-time Visualization**: See the A* algorithm in action as it finds the optimal path on the grid.
  
Built using **Raylib**, a simple and easy-to-use library for handling graphics. You can learn more about it [here](https://www.raylib.com/).

### Screenshots

![App Screenshot](https://github.com/AndreSbro03/AStar/blob/main/screen.png)

## Installation & Running the Program

### Prerequisites
Ensure you have **Raylib** installed on your machine. You can find installation instructions on the official [Raylib website](https://www.raylib.com/).

### Installation
To download and run the project, follow these steps:

1. Clone the repository and navigate to the project folder.
2. Make the build script executable and run it:
  
  ```bash
    chmod +x build.sh
    ./build.sh
  ```

After building, you can run the program with:

  ```bash
    ./AStarAlgorithm
  ```

### Optional Commands

You can run the program with additional options for different use cases:

- **Run without Raylib logs**:  
  Suppress Raylib's log output by adding `true` as an argument:
  
  ```bash
  ./AStarAlgorithm true
  ```

- **Run as deamon**:  
  To run the program in the background (daemon mode), use the following command:

  ```bash
    ./RunAsDeamon true
  ```

## Globals and Variables

### Global Parameters

- **`S_DIMX`** & **`S_DIMY`**: Define the dimensions of the window (screen size).
- **`C_DIM`**: Specifies the cell size in pixels, determining the grid resolution.
- **`DST`**: This constant indicates the cost of moving between two consecutive cells. A higher value means more paths are explored, resulting in a more accurate solution but slower performance. Values between `0.5` and `0.95` work best.
- **`R_RANDOMNESS`**: The probability that a cell is a solid block, defined as `1/R_RANDOMNESS`.
- **`REG_POINT`**: If enabled, pressing "R" will regenerate the start and end points randomly.
- **`PRINT_FINAL_ROUTE`**: If enabled, prints the list of nodes in the final path to the terminal.
- **`RANDOM_SPAWN`**: When set to `true`, the start and end points are generated randomly. If `false`, they are placed at opposite corners of the grid.
- **`STEP_DST`**: If `true`, the distance function operates on a grid (Manhattan distance). If `false`, the map behaves as a graph, calculating distances using the Pythagorean theorem (Euclidean distance), which is slower.

### Variables

- **`pause`**: Initially set to `true`, meaning the algorithm is paused by default and will only start when you press "Enter".
- **`draw_final_route`**: If `true` (default), the final route is visually displayed on the grid after the algorithm completes.


## Acknowledgements

This project was inspired by the [A* algorithm description](https://en.wikipedia.org/wiki/A*_search_algorithm) and aims to provide an interactive visualization of this classic pathfinding algorithm. 

Special thanks to the **Raylib** library for simplifying the graphical interface and making real-time rendering possible. You can learn more about Raylib [here](https://www.raylib.com/).

Feel free to contribute, explore the code, or provide feedback to help improve the project!
