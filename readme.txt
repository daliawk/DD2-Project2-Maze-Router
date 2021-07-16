Course: CSCE 3304 Digital Design II
Project: Maze Router
Developed by: Dalia El-Naggar 900191234
	      Nada Atia	      900193555

github link: https://github.com/daliawk/DD2-Project2-Maze-Router

Contents:
- DEF_file.txt (A text file with the layer dimensions, nets' pins and obstacles coordinates)
- Maze_Router.cpp (takes DEF_file.txt to produce an output file with the routed nets)
- Visualize_Nets.py (takes both previously mentioned input and output files and visualizes the nets) 
- "Test Cases.pdf" which contains 10 test cases their results.
- "DD2 Project 2 Presentation.pdf": a PowerPoint presentation of the project

Assumptions:
- Obstacles exist in both layers
- Input file contents does not contain any blank spaces.
- Coordinates of obstacles and pins are 0-based.
- The x-coordinate represents rows and y-coordinate represents columns.
- There are only 2 metal layers.

How to use:
- Open DEF_file.txt and set the layer dimensions, nets' pins' and obstacles' coordinates.
- Run Maze_Router.cpp to run Lee's Algorithm (Make sure that DEF_file.txt is in the same folder)
- The program will ask for the penalty of using vias and moving in an unpreferred direction.
- Run Visualize_Nets.py (Make sure that DEF_file.txt and the output file from the previous step are in the same folder)
  Notes:
  - Black cells are obstacles
  - All cells marked "S" are the source pins of their respective net.
  - All cells marked "T" are the target pins of their respective net.
  - All cells marked "@" are via connectors.
  - If the number of rows >= 20, each layer gets represented in a seperate figure.

Limitations:
- The program works on only two metal layers
- If the dimensions are mxn, then the time complexity and the space complexity are O(mxn).
- The visualization of large dimensions are not clear due to small cell sizes