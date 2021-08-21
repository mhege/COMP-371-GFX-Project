Project Team 9
--------------
This is a graphics programming project for 'COMP 371 - Computer Graphics'. 
It incorporates fundamental aspects of computer graphics such as modeling, camera control, lighting, texture mapping, and animation.


Environment
-----------
To avoid compatibility issues, it is recommended to run this project on a lab computer running Windows.
This program can be run on Windows, Mac or Linux computers; however, modifications may be required for non-lab computers.


Getting Started
---------------

For Lab Instructors:
Due to size restrictions in the submission system, the .cpp, project, and sln files will be solely provided. 
To run the project, insure that these provided files are loaded within the provided lab framework folder in Visual Studio.
Make sure that the subfolder ThirdParty is included as it should be.

Clone or download the project and open it with Visual Studio or another c++ IDE of your choice. Open gfxProject.cpp, compile and then run. 
A windows with the models will appear. Use the control keys to navigate through the 3d environment or to rotate or move models.


Controls
--------
Toggle texture:
x, on/off

Toggle shadow:
SHIFT + b, on/off

Re-positioning models:
SPACE, Re-positioning at random location

Shearing displacement on models:
SHIFT + up arrow, Back shearing displacement
SHIFT + right arrow, Right shearing displacement
SHIFT + down arrow, Front shearing displacement
SHIFT + left arrow, Left shearing displacement

Switch models:
1, R4 (Center)
2, L0 (Top right)
3, A8 (Top left)
4, V4 (back right)
5, A6 (back left)

Switch view to models:
c, R4 (Center)
v, L0 (Top right)
b, A8 (Top left)
n, V4 (back right)
m, A6 (back left)

Draw triangle, line, or points:
Select model with 1-5 then,
t, Draw in triangles
l, Draw in lines
p, Draw in points

Scale model:
Select model with 1-5 then,
u, Scale up
j, Scale down

Translate model:
Select model with 1-5 then,
SHIFT+a, translate left
SHIFT+d, translate right
SHIFT+w, translate up
SHIFT+s, translate down

Rotate model:
Select model with 1-5 then,
a, rotate left
d, rotate right
w, rotate up
s, rotate down

Rotate world orientation:
left arrow, anti-clockwise positive x axis
right arrow, clockwise positive x axis
up arrow, anti-clockwise positive y axis
down arrow, clockwise positive y axis
HOME, reset to default

Pan, Tilt, Zoom:
hold right mouse + mouse along x, pan (camera along x)
hold middle mouse + mouse along y, tilt (camera along y)
hold left mouse + mouse along y, zoom (camera along z)


Built With
----------
C++ - Programming Language
OpenGL - 2D and 3D vector API
Visual Studio - IDE


Authors
-------
Marc Hegedus
Nalveer Moocheet
Noah El Hayek
Sean Howard
Tzvi Filler
