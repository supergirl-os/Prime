# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = "D:\software\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\software\CLion 2021.2.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug

# Utility rule file for check.

# Include any custom commands dependencies for this target.
include test/CMakeFiles/check.dir/compiler_depend.make

# Include the progress variables for this target.
include test/CMakeFiles/check.dir/progress.make

test/CMakeFiles/check:
	cd /d D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug\test && "D:\software\CLion 2021.2.3\bin\cmake\win\bin\ctest.exe" --verbose

check: test/CMakeFiles/check
check: test/CMakeFiles/check.dir/build.make
.PHONY : check

# Rule to build all files generated by this target.
test/CMakeFiles/check.dir/build: check
.PHONY : test/CMakeFiles/check.dir/build

test/CMakeFiles/check.dir/clean:
	cd /d D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug\test && $(CMAKE_COMMAND) -P CMakeFiles\check.dir\cmake_clean.cmake
.PHONY : test/CMakeFiles/check.dir/clean

test/CMakeFiles/check.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\test D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug\test D:\本科课内学习资料集锦\大三上学习资料包\数据库系统原理\scudb_initial\scudb_initial\cmake-build-debug\test\CMakeFiles\check.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/check.dir/depend
