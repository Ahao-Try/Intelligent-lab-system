# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/cmake-3.21.4/bin/cmake

# The command to remove a file.
RM = /opt/cmake-3.21.4/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lzh/Intelligent-lab-system/Intelligent-lab-system

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build

# Utility rule file for build_.

# Include any custom commands dependencies for this target.
include driver/CMakeFiles/build_.dir/compiler_depend.make

# Include the progress variables for this target.
include driver/CMakeFiles/build_.dir/progress.make

driver/CMakeFiles/build_:
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /usr/bin/make -C /home/lzh/100ask_imx6ull-sdk/Linux-4.9.88 M=/home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver modules
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E copy_if_different /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.o /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/modules/
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E remove /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.o
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E copy_if_different /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.ko /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/modules/
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E remove /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.ko
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E copy_if_different /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.mod.c /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/modules/
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E remove /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/*.mod.c
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E copy_if_different /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/Module.symvers /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/modules/
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E remove /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/Module.symvers
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E copy_if_different /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/modules.order /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/modules/
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && /opt/cmake-3.21.4/bin/cmake -E remove /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver/modules.order

build_: driver/CMakeFiles/build_
build_: driver/CMakeFiles/build_.dir/build.make
.PHONY : build_

# Rule to build all files generated by this target.
driver/CMakeFiles/build_.dir/build: build_
.PHONY : driver/CMakeFiles/build_.dir/build

driver/CMakeFiles/build_.dir/clean:
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver && $(CMAKE_COMMAND) -P CMakeFiles/build_.dir/cmake_clean.cmake
.PHONY : driver/CMakeFiles/build_.dir/clean

driver/CMakeFiles/build_.dir/depend:
	cd /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lzh/Intelligent-lab-system/Intelligent-lab-system /home/lzh/Intelligent-lab-system/Intelligent-lab-system/driver /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver /home/lzh/Intelligent-lab-system/Intelligent-lab-system/build/driver/CMakeFiles/build_.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : driver/CMakeFiles/build_.dir/depend

