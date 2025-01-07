# Rest Control
Simple QT user interface to generate rest commands.

# Build Prerequisites

- Microsoft Visual Studio Community 2022 (64-bit)
- Qt (5.15.2)
- Cmake (3.29.3)
- SDL2 2.0.8

# Generate MSVS Build

 1. Open CMAKE GUI
 2. Set source code path to your local repository (i.e. C:/rest_control)
 3. Set build the binaries path to <local repository>\build. (i.e C:/rest_control/build)
 4. Click Add Entry button. The Add Cache Entry window will pop-up.
    * Set variable CMAKE_PREFIX_PATH to QT version for MSVS2019 64-bit (i.e. C:/Qt/5.15.2/msvc2019_64)
    * Click OK.
 5. Click Configure.
    If prompted if directory should be created, select Yes.
 6. Window to select MSVS version and platform will pop up. Select the following options:
    * Specify the generator for this project: Visual Studio 17 2022
    * Optional platform for generator: x64
    * Click Finish
 7. CMAKE process will start to configure your project.
    Once it completes, it will print "Configure done".
 8. Once configuration completes, click Generate
 9. Once Generate completes, click Open Project.

# Preview

 ![Screenshot](/rest_control_preview.jpg)

## Author info

[Linkedin](https://www.linkedin.com/in/miguel-a-duenas-sr1)