# 2D-SP
Heuristic and Metaheuristic algorithms to solve 2D SP problem.<br />

Here implemented Heuristic and Metaheuristic algorithms to solve 2D Strip Packing problem.<br />
2D-SP - pack given rectangles to a semi-infinite strip with given width minimizing occupied height.<br />
Used heuristic algorithm is PBF - Priority Best Fit.<br />
Used metaheuristic algorithm is Simulated Annealing.<br />

Project was written in C++ and also used python script draw.py to draw solutions.<br />
Dependencies are already in repository such as IBM's CPLEX includes and libraries (in "Dependencies" directory) and graphics.py.<br />
draw.py also uses PIL to save images, this library needs to be installed using for example pip "pip install Pillow".<br />
"files" directory contains created by program files.<br />
"generated" directory contains test problems for compared algorithms.<br />
"diploma_and_literature" directory contains diploma itself (in Russian) and literature used while developing.<br />
