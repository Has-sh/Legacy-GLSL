# 3D Object Transformation Viewer

This project is a 3D Object Transformation Viewer that loads 3D objects from an OFF (Object File Format) file and allows users to perform various transformations on the object, such as translation, rotation, scaling, and shearing. The project uses OpenGL for rendering and ImGui for the graphical user interface.

## Features

- Load 3D objects from OFF files.
- Apply transformations including:
  - Translation (Move the object in X, Y, or Z directions).
  - Rotation (Rotate the object around the X, Y, or Z axes).
  - Scaling (Resize the object along the X, Y, or Z axes).
  - Shearing (Shear the object along the X, Y, or Z axes).
- Reset transformations to restore the object to its original state.
- Real-time rendering with ImGui controls for easy interaction.

## Dependencies

- [OpenGL](https://www.opengl.org/)
- [GLUT](https://www.opengl.org/resources/libraries/glut/)
- [ImGui](https://github.com/ocornut/imgui) (included in the `vendor/` directory)
- C++11 or later

## Installation

- Open with Visual Studio using ModelingTransformation.sln
  
## Usage

- **Loading a 3D Object**:
  - Provide the path to an OFF file as a command-line argument when running the program.
  
- **Applying Transformations**:
  - Use the ImGui controls to apply translation, rotation, scaling, and shearing to the loaded 3D object.

- **Resetting Transformations**:
  - Use the reset button in the ImGui interface to restore the object to its original state.

## File Structure

- `main.cpp`: The main source file containing the OpenGL setup, rendering loop, and transformation logic.
- `vendor/`: Contains third-party libraries including ImGui.
- `models/`: Directory where you can place your OFF files.
- `README.md`: This documentation file.

## Known Issues

- The application currently assumes that the OFF file is correctly formatted. Invalid or corrupted files may cause unexpected behavior.
- Performance may vary depending on the complexity of the 3D model.

## Acknowledgments

- [ImGui](https://github.com/ocornut/imgui) by Omar Cornut for providing the GUI library.
- OpenGL and GLUT for enabling 3D rendering.
