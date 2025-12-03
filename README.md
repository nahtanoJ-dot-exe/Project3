# New York Traversal
This project uses three different algorithms to find the shortest path between any two points in New York. The purpose of this
project is to compare each of the algorithms' performances in terms of time as well as to showcase visually how these algorithms
work. 

## Description
We used DIMACS to implement a map of New York, then converted that map into an undirected weighted graph with over 266,000
nodes and 733,000 edges. The three pathfinding algorithms that can be compared are:

---
### A* Search Algorithm
### Dijkstra's Shortest Path Algorithm
### Two-Way Dijkstra's Shortest Path Algorithm
---

## How to Use 
Once the program is running, the user has multiple options to change the source vertex, destination vertex, as well as which 
algorithm will be used. Here is what each key does:
|Key|Function|
|-|-|
|A|Move destination node (backwards)|
|D|Move destionation node (forwards)|
|Left Arrow|Mode source node (backwards)|
|Right Arrow|Move source node (forwards)|
|1|Set algorithm to Dijkstra's Shortest Path|
|2|Set algorithm to Two-Way Dijkstra's Shortest Path|
|3|Set algorithm to A* Search|
|R|Reset|
|Space Bar|Run Algorithm|

Once users use each algorithm, the time that it took the respective algorithm to find a path as well as the length of the path
are showcased. 
