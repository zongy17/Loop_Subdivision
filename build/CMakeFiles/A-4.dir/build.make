# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\cmake-3.21.2-windows-x86_64\bin\cmake.exe

# The command to remove a file.
RM = D:\cmake-3.21.2-windows-x86_64\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\2021Fall\ComputerGraphics\homework\A-4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\2021Fall\ComputerGraphics\homework\A-4\build

# Include any dependencies generated for this target.
include CMakeFiles/A-4.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/A-4.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/A-4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/A-4.dir/flags.make

CMakeFiles/A-4.dir/src/glad.c.obj: CMakeFiles/A-4.dir/flags.make
CMakeFiles/A-4.dir/src/glad.c.obj: CMakeFiles/A-4.dir/includes_C.rsp
CMakeFiles/A-4.dir/src/glad.c.obj: ../src/glad.c
CMakeFiles/A-4.dir/src/glad.c.obj: CMakeFiles/A-4.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\2021Fall\ComputerGraphics\homework\A-4\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/A-4.dir/src/glad.c.obj"
	D:\mingw64\bin\x86_64-w64-mingw32-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/A-4.dir/src/glad.c.obj -MF CMakeFiles\A-4.dir\src\glad.c.obj.d -o CMakeFiles\A-4.dir\src\glad.c.obj -c D:\2021Fall\ComputerGraphics\homework\A-4\src\glad.c

CMakeFiles/A-4.dir/src/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/A-4.dir/src/glad.c.i"
	D:\mingw64\bin\x86_64-w64-mingw32-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E D:\2021Fall\ComputerGraphics\homework\A-4\src\glad.c > CMakeFiles\A-4.dir\src\glad.c.i

CMakeFiles/A-4.dir/src/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/A-4.dir/src/glad.c.s"
	D:\mingw64\bin\x86_64-w64-mingw32-gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S D:\2021Fall\ComputerGraphics\homework\A-4\src\glad.c -o CMakeFiles\A-4.dir\src\glad.c.s

CMakeFiles/A-4.dir/src/main.cpp.obj: CMakeFiles/A-4.dir/flags.make
CMakeFiles/A-4.dir/src/main.cpp.obj: CMakeFiles/A-4.dir/includes_CXX.rsp
CMakeFiles/A-4.dir/src/main.cpp.obj: ../src/main.cpp
CMakeFiles/A-4.dir/src/main.cpp.obj: CMakeFiles/A-4.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\2021Fall\ComputerGraphics\homework\A-4\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/A-4.dir/src/main.cpp.obj"
	D:\mingw64\bin\x86_64-w64-mingw32-g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/A-4.dir/src/main.cpp.obj -MF CMakeFiles\A-4.dir\src\main.cpp.obj.d -o CMakeFiles\A-4.dir\src\main.cpp.obj -c D:\2021Fall\ComputerGraphics\homework\A-4\src\main.cpp

CMakeFiles/A-4.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/A-4.dir/src/main.cpp.i"
	D:\mingw64\bin\x86_64-w64-mingw32-g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\2021Fall\ComputerGraphics\homework\A-4\src\main.cpp > CMakeFiles\A-4.dir\src\main.cpp.i

CMakeFiles/A-4.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/A-4.dir/src/main.cpp.s"
	D:\mingw64\bin\x86_64-w64-mingw32-g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\2021Fall\ComputerGraphics\homework\A-4\src\main.cpp -o CMakeFiles\A-4.dir\src\main.cpp.s

# Object files for target A-4
A__4_OBJECTS = \
"CMakeFiles/A-4.dir/src/glad.c.obj" \
"CMakeFiles/A-4.dir/src/main.cpp.obj"

# External object files for target A-4
A__4_EXTERNAL_OBJECTS =

A-4.exe: CMakeFiles/A-4.dir/src/glad.c.obj
A-4.exe: CMakeFiles/A-4.dir/src/main.cpp.obj
A-4.exe: CMakeFiles/A-4.dir/build.make
A-4.exe: CMakeFiles/A-4.dir/linklibs.rsp
A-4.exe: CMakeFiles/A-4.dir/objects1.rsp
A-4.exe: CMakeFiles/A-4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\2021Fall\ComputerGraphics\homework\A-4\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable A-4.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\A-4.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/A-4.dir/build: A-4.exe
.PHONY : CMakeFiles/A-4.dir/build

CMakeFiles/A-4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\A-4.dir\cmake_clean.cmake
.PHONY : CMakeFiles/A-4.dir/clean

CMakeFiles/A-4.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\2021Fall\ComputerGraphics\homework\A-4 D:\2021Fall\ComputerGraphics\homework\A-4 D:\2021Fall\ComputerGraphics\homework\A-4\build D:\2021Fall\ComputerGraphics\homework\A-4\build D:\2021Fall\ComputerGraphics\homework\A-4\build\CMakeFiles\A-4.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/A-4.dir/depend
