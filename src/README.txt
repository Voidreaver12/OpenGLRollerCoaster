Adrien Perinet
Ian Tobiason
Noah Deibert

Group: Ra-ller Coaster
Park Area: Sphinx

Assignment: Roller Coaster Tycoon

Explore the Ra-ller Mega-Coaster and marvel at the power of Ra and his disciples!
This project allows you to see the coaster in action and take control of the Wanderer.

Keys/Usage
  Move the camera by click-dragging. Use Ctrl-click dragging to move the camera forwards or backwards.

  1 Use the freecam.
  2 Use the arcball cameras. Press this key multiple times to cycle through each Hero.
  3 Use the first-person cameras. Press this key multiple times to cycle through each Hero.
  4 Toggle control for the Wanderer. Once activated:
		W (forward)
	A (rotate left) D (rotate right)
		S (backwards)
  Q/Esc Exit
  
Compilation: just type make!  
Run with ./RCT.exe <file> where the file can either be
"splashland.csv" or "loopofdeath.csv"

File Format:
<number of control points for roller coaster track track>
<control point x>,<control point y>,<control point z>
...
<number of Bézier Surfaces for ground>
<number of control points for bezier surface>
<control point x>,<control point y>,<control point z>
...
<number of trees>
<number of mushrooms>
<tree/mushroom height> <tree/mushroom x> <tree/mushroom y> <tree/mushroom z>
...

Responsibility Distribution:
Adrien: Bezier surface, file format
Ian: Roller coaster
Noah: Cameras

This assignment probably took about 30 hours total
Labs helped out great for this assignment, but this assignment obviously took quite a bit of exploration: 7/10
This assignment was pretty fun! 9/10