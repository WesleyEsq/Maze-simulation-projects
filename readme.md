# Maze simulator projects

Made by the students Wesley Esquivel and Diego Castro

- **Last modification:** 23rd of march 2024

- **Tecnológico de Costa Rica, Análisis de algoritmos, group 40**


This is a project written entirely in C with the purpose to generate and test many well known maze solving algorithms, as well as generating an interactive Maze made from absolute scratch to work efficiently, only using some functions to draw the graphics.


### Instalation

The project runs on ubuntu and debian, probably other distros as well if you install the necessary dependencies the project includes now an install to make the executable with a single bash script, titled "install.sh".

To execute the instalation you need to do the following:

1. Download or clone this repository in your linux computer

2. Find the directory that was downloaded and make a terminal on it
   
   + You can do this by either rigth cliking the place where it was downloaded and choosing the open terminal in this directory with the file manager
   
   + Or by opening a terminal and getting to that directory inside it.

3. Type the following commands on the terminal:

``` 
chmod x+ install.sh
```

```
./install.sh
```

If the installation was succesful, then you can execute the binary called "maiz" in the maze directory. You can do so either from the terminal too by a ./maiz/maze or with the file system of your operating system again.


### Controls and interface

To generate a map you must go to the first button and choose the dimensions of the map, the maximum size was 2048 only because it was a hardlimit by the professor, the algorithm was able to scale up to 20 thousand x 20 thousand cells without showing a decrease in performance at the cost of some dynamic memory usage. Every bit is used to the fullest extend to ensure it's not an issue.

To move in the maze you need to click from one point to another in the screen to "drag" the map with your cursor towards the desired location you cliked next. As if it was a regular map. The values and position you are in are shown on the left. Clicking the iddle wheel should send you back to the begining of the maze, using the wheel to scroll up or down alters the zoom level of the map.

To run the simulation of the maze solving algorithms you first will need to make a map of the labyrithn, you can then press the last button in the main menu to analyse the execution processs of each algorithm inside the map you just created in the maze window. Each is represented by a color and a line that represents it's path. 

You can either execute the algorithms at a slow pace to see how they move (specially in smaller mazes) or execute them as fast as they can run. There is a limitation of only moving 1 space per microsecond. On bigger mazes you can zoom out to see how they flood the maze with paths to find a way out with several colors and their combinations.

You can save and load any particular maze if you go to the save and load menu, you can select where to save or load the mazes. They are stored in files the program can read quickly.

### About this project

This was a product of a two step project in the course of algorithm analysis long ago. It's a two coponent system, a  GUI menu from where to manage the program and start the simulations and then a window to view the program running. The project is way more advanced than it needed to be, many concepts from GIS systems were used to implement a solution that is efficient in memory usage and time complexity, both linear in complexity of O(n), with n being the ammount of cells created and stored in memory.

The algorithms displayed are the following:

+ **Random mouse:** Will make random moves

+ **Left hand:** Will follow the left hand rule to get out of the maze

+ **Rigth hand:** Will follow the rigth hand rule instead

- **Pledge Algorithm:** Follows a preferred direction, using a wall follower (counting turns until the sum is zero) to bypass obstacles.

- **Trémaux Algorithm:** Systematically explores by marking paths (once when entering, twice when exiting/dead-end) and never entering a path marked twice.

- **Fattah Algorithm:** Based on a paper by the author's name, tries to always move towards the goal considering it's manhatans distance. When it hits a dead end, the mouse will either use the rigth hand or left hand rule based on if the next movement is on the rigth or left of the front of the goal

That last one was achieved using polar coordinates and heavy use of trigonometry, it was fun to figure that out without any help at all from any source, not even the paper.

### Technical details

The simulation uses cairo graphics to display the map of the maze in real time. The map is done using two layers, one for logic and for the mice to live in and another to generate a raster map with a matrix to draw on the screen much more quickly.

The map is generated using a paper that both me and my teammate had read a couple of years ago and now I'm not sure what it was, I will cite it here later. The map was done absolutely from scratch, only asking gtk to give me a window and get cursor inputs. Each algorithm also had an academic source that I do not have now.

All of the project and it's thousands of lines of code were wrote by hand and with the source of documentations and forums asking for common compiling errors, and we only had pseudocode for some algorythms available to us.

There is a minor bug involving the counters, they are not labeled correctly with each other, it was the only mistake the project had and for the sake of keeping it just the way it was, I'm leaving it there.
