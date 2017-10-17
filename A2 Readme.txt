.........................................................................................................
	Assignment 2
.........................................................................................................

.........................................................................................................
	To run:
.........................................................................................................
- Open sln with Visual Studio 2015
- Starup solution should be A2
- Start debugging

.........................................................................................................
	Controls:
.........................................................................................................
ESC		toggle the cursor visibily
BACKSPACE	reset the camera and the HeightMapManager
Z		cycle between points indices of the HeightMapManager's EBO (sub points, catmullrom x, and smooth)
X		toggle between original points and new points.
C*		cycle render mode (triangles or points)
W, A, S, D	controls the camera, left, down and right respectively

MOUSE LEFT	enable camera movement when held down


* sadly not functional...

.........................................................................................................
	Extra Credit Implementation:
.........................................................................................................
- HeightMapManager has another VAO with an EBO that contains all the points provided by the image.