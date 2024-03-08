==== Build Instructions ====

This is a 64-bit Windows application. In order to compile this, you need Visual Studio 2017+ with the
"Desktop Development with C++" option included.

The build system is a simple batch file that invokes the MSVC compiler. You will need a command
prompt with the correct environment variables set, in order for the compiler to be present. There are
two ways to achieve this:

1) Using the Visual Studio Developer Command Prompt

- Open the start menu, search for "x64 Native Tools Command Prompt for VS 20XX" and open it.
- Navigate to the project root directory.
- Run the build.bat batch file by typing "build".

2) Using a regular Command Prompt

- Open the start menu, search for "Command Prompt" and open it.
- Navigate to the project root directory.
- Run the setup.bat batch file that is inside the misc directory by typing "misc\shell". This scipt will use vswhere
  to locate the Visual Studio installation directory and setup the environment variables for x64 MSVC compiler.
- Run the build.bat batch file by typing "build".

After compiling the application, the two executables "gfx_debug.exe" and "gfx_release.exe" will appear in the
"run_tree" directory. In order the the application to find its assets, it must be in the same directory with the
"assets" directory.

==== Controls ====

H: Decreases the rotation of the sphere by 50 degrees per second held.
J: Increases the rotation of the sphere by 50 degrees per second held.

When the camera is active:
  - Escape: Deactivate the camera.
  - Mouse: Orient the camera.
  - WASD: Move the camera.
  - Left Shift: Move the camera up.
  - Left Control: Move the camera down.
  - Mouse Wheel: Change the camera movement speed.

When the camera is inactive:
  - Left Mouse Button: Activate the camera.
  - Escape: Close the application.

By default, the camera is inactive when the application is launched.

==== Notes ====

There has been one change with regards to the assignment. Changing the rotational speed of the sphere by 0.05 per
keystroke, seemed too low and unresponsive, thus not showcasing the dynamic transform well. So instead, the change
of speed was bumped up to 50 degrees per second where the key is held. Presice control of the sphere rotation is not
quite possible now, but there is a greater sense of change of the sphere's rotational speed.

The general structure of the application is quite simple so it won't be discussed, but here are some main points:

1) In order to keep the application as simple as possible, there were no libraries used appart from the C-Runtime
Library. The Win32 API was used to interface with the operating system, for window creation, file reading, memory
allocation and timing. WSL was used as the OpenGL-Windowing API interface. A small number of widely available WSL
extensions was used in order to craete a true modern OpenGL context. Lastly, the small number of OpenGL functions
that were used, are loaded from the DLL by hand and the constants are from the official Khronos OpenGL API registry.

2) For loading meshes, a custom OBJ importer was written. It supports positions, texture coordinates and normals for
triangulated meshes. It also supports groups with different materials. Materials are imported from MTL files. For
loading texture files, a custom TGA importer was written. It supports BGR and BGRA formats, with run-length encoding
as well as raw uncompressed values.

3) The math library was written from scratch and includes the following structures with only the required
functionality:

- Simple Operations (Degrees to Radians, Clamp)
- 2D Integer Vectors (Subraction, Addition)
- 3D Floating Point Vectors (Negation, Scalar-Multiplication, Addition, Subtraction, Length, Normalization)
- 4D Floating Point Vectors
- Quaternions (Conjugate, Length, Normalization, Multiplication, Axis Angle Rotation, 3D Vector Rotation)
- 4x4 Matrix (Multiplication, Perspective Projection, Translation, Traslation Decomposition, Scaling, Quaternion to Matrix)

4) All rotations are represented using quaternions. That includes mesh and camera orientation.

5) All transformations take the frame time into account. This way, every movement is framerate-independent.

6) All graphics assets that are loaded, are then uploaded to video memory and are freed from system memory.

7) A single vertex and pixel shader compbination is used for every object in the scene. This uber shader supports lit
and unlit objects, textured and non-textured materials.

8) The light in the scene is a simple point light that follows the trajectory of the sphere.

9) The UV sphere is generated using a modified version of the code supplied. Since the application is more C-like,
it was moved into a simple function. In addition, since only the vertex positions are needed, they are the only things
generated.

10) Vertical synchronization is enabled by default. Otherwise the application would run at 5000+ frames per second,
stressing the GPU for no reason.

11) The frame time and sphere rotation speed are written out int the window title, for easy readout.

12) For the most responsive mouse input, raw input is used.

13) The application supports window resizing, though currently it is a bit expensive, since the projection matrix
is being recalculated each frame instead on resize events.
