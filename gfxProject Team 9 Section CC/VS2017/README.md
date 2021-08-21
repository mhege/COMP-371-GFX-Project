# Project Team 9

This is a graphics programming project for 'COMP 371 - Computer Graphics'. It incorporates fundamental aspects of computer graphics such as modeling, camera control, lighting, texture mapping, and animation.

### Environment

To avoid compatibility issues, it is recommended to run this project on a lab computer running Windows.<br/> 
This program can be run on Windows, Mac or Linux computers; however, modifications may be required for non-lab computers.

### Getting Started

For Lab Instructors:<br/>
Due to size restrictions in the submission system, the `.cpp` , project, and sln files will be solely provided. To run the project, insure that the provided files are loaded within the `Lab02FullFramework` folder in `Visual Studio`. Make sure that the subfloder `ThirdParty` is included as it should be.

`clone` or download the project and open it with Visual Studio or another c++ IDE of your choice. Open 'gfxProject.cpp', compile and then run. A windows with the models will appear. Use the control keys to navigate through the 3d environment or to rotate or move models.


### Controls

Toggle texture:<br/>
`x`, on/off<br/>
<br/>
Toggle shadow:<br/>
`SHIFT + b`, on/off<br/>
<br/>
Shearing displacement on models:<br/>
`SHIFT + up arrow`, Back shearing displacement<br/>
`SHIFT + right arrow`, Right shearing displacement<br/>
`SHIFT + down arrow`, Front shearing displacement<br/>
`SHIFT + left arrow`, Left shearing displacement<br/>
<br/>
Re-positioning models:<br/>
`SPACE`, Re-positioning at random location<br/>
<br/>
Switch models:<br/>
`1`, R4 (Center)<br/>
`2`, L0 (Top right)<br/>
`3`, A8 (Top left)<br/>
`4`, V4 (back right)<br/>
`5` , A6 (back left)<br/>
<br/>
Switch view to models:<br/>
`c`, R4 (Center)<br/>
`v`, L0 (Top right)<br/>
`b`, A8 (Top left)<br/>
`n`, V4 (back right)<br/>
`m`, A6 (back left)<br/>
<br/>
Draw triangle, line, or points:<br/>
Select model with 1-5 then,<br/>
`t`, Draw in triangles<br/>
`l`, Draw in lines<br/>
`p`, Draw in points<br/>
<br/>
Scale model:<br/>
Select model with 1-5 then,<br/>
`u`, Scale up<br/>
`j`, Scale down<br/>
<br/>
Translate model:<br/>
Select model with 1-5 then,<br/>
`SHIFT + a`, translate left<br/>
`SHIFT + d`, translate right<br/>
`SHIFT + w`, translate up<br/>
`SHIFT + s`, translate down<br/>
<br/>
Rotate model:<br/>
Select model with 1-5 then,<br/>
`a`, rotate left<br/>
`d`, rotate right<br/>
`w`, rotate up<br/>
`s`, rotate down<br/>
<br/>
Rotate world orientation:<br/>
`left arrow`, anti-clockwise positive x axis<br/>
`right arrow`, clockwise positive x axis<br/>
`up arrow`, anti-clockwise positive y axis<br/>
`down arrow`, clockwise positive y axis<br/>
`HOME`, reset to default<br/>
<br/>
Pan, Tilt, Zoom:<br/>
hold right mouse + mouse along `x`,  pan (camera along x)<br/>
hold middle mouse + mouse along `y`, tilt (camera along y)<br/>
hold left mouse + mouse along `y`, zoom (camera along z)<br/>


## Built With

- [C++](https://isocpp.org/) - Programming Language
- [OpenGL](https://www.opengl.org/) - 2D and 3D vector API
- [Visual Studio](https://visualstudio.microsoft.com/) - IDE

## Authors

- Marc Hegedus
- Nalveer Moocheet
- Noah El Hayek
- Sean Howard
- Tzvi Filler
