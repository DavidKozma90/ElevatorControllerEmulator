# How to build
The project build framework is using premake5 to generate makefiles.
The integration was done in Visual Studio Code v1.101.2 using MinGW-W64 2.0.0 toolkit in Windows 10 environment.
The project is cross-platform and can be built in Visual Studio as well.

## Supported Platforms
The framework supports the main 3 desktop platforms:
* Windows
* Linux
* MacOS

## VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode (see below).

* Download the project as a .zip or clone the main branch of the repository from GitHub
* Rename the folder to your project name (default is ElevatorControllerEmulator)
* Open the folder in VSCode
* Run the build task ( CTRL+SHIFT+B or F5 )
* You are good to go

## Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio
## Using MinGW-W64
* Double click the `build-MinGW-W64.bat` file
* CD into the folder in your terminal
* run `make`
* You are good to go

### Note on MinGW-64 versions
Make sure you have a modern version of MinGW-W64 (not mingw).
The best place to get it is from the W64devkit from
https://github.com/skeeto/w64devkit/releases

Make sure you have added the toolkit bin folder path to your path environment variable so that the compiler that came with raylib can be found.

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

## Microsoft Visual Studio
* Run `build-VisualStudio2022.bat`
* double click the `.sln` file that is generated
* develop your game
* you are good to go

## Linux Users
* CD into the build folder
* run `./premake5 gmake2`
* CD back to the root
* run `make`
* you are good to go

## MacOS Users
* CD into the build folder
* run `./premake5.osx gmake2`
* CD back to the root
* run `make`
* you are good to go

## Output files
The built code will be in the bin dir

# Directory Overview

- **.vscode/**  
  VS Code workspace settings.
- **bin/**  
  Output directory for compiled binaries (created after build).
- **resources/**  
  Contains the original project assignment documentation.
- **src/**  
  Main source code for the emulator, including controller logic, test framework, and utilities. See detailed project documentation here:
  [src/README.md](src/README.md)
- **build/**  
  Build scripts and configuration files (e.g., `premake5.lua`).

 # How to run

 After building the project run it in Debug mode (Release mode works fine as well but asserts won't trigger). You need to have a default console setup in order to see the runtime results.

 ## VSCode and Visual Studio Users
 * Go to src folder and open main.c
 * Press F5 to run
 * You'll be prompted with a dialog interface in the console/terminal how to use the elevator controller emulator.

 ## How to Use the Elevator Controller Emulator

After building and running the program, you will interact with a simple console-based menu. The main dialog is implemented in `src/main.c` and provides the following options:

### Console Menu Options

- **1. Run simple example simulation**  
  Prompts you to enter the elevator's starting position (0-5) and the destination floor (0-5). The emulator will simulate the elevator's behavior for this scenario and print each step of the process.

- **2. Load and display default program**  
  Loads the default elevator control (firmware) program into memory and prints its layout for inspection.

- **3. Print program memory**  
  Displays the current contents of the program (firmware) memory.

- **4. Run validation tests**  
  Executes a suite of built-in validation tests to verify the correct operation of the elevator controller logic.

- **x. Exit**  
  Exits the emulator.

### Example Usage

When you start the program, you will see a menu like this:
```console
Elevator Controller Emulator - Menu
1. Run simple example simulation 
2. Load and display default program
3. Print program memory 
4. Run validation tests
x. Exit
Select an option (and press Enter):
```
Enter the number or letter corresponding to your desired action and follow the prompts. For simulation, ensure you enter floor numbers between 0 and 5 as requested.












