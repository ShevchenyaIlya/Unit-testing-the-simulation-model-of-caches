# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/clion-2019.3.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2019.3.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/shevchenya/CLionProjects/CourseWorkCache

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug

# Include any dependencies generated for this target.
include google_tests/lib/googletest/CMakeFiles/gtest.dir/depend.make

# Include the progress variables for this target.
include google_tests/lib/googletest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include google_tests/lib/googletest/CMakeFiles/gtest.dir/flags.make

google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: google_tests/lib/googletest/CMakeFiles/gtest.dir/flags.make
google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: ../google_tests/lib/googletest/src/gtest-all.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/lib/googletest/src/gtest-all.cc

google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/lib/googletest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/lib/googletest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.o"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

lib/libgtestd.a: google_tests/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
lib/libgtestd.a: google_tests/lib/googletest/CMakeFiles/gtest.dir/build.make
lib/libgtestd.a: google_tests/lib/googletest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgtestd.a"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
google_tests/lib/googletest/CMakeFiles/gtest.dir/build: lib/libgtestd.a

.PHONY : google_tests/lib/googletest/CMakeFiles/gtest.dir/build

google_tests/lib/googletest/CMakeFiles/gtest.dir/clean:
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : google_tests/lib/googletest/CMakeFiles/gtest.dir/clean

google_tests/lib/googletest/CMakeFiles/gtest.dir/depend:
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shevchenya/CLionProjects/CourseWorkCache /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/lib/googletest /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/lib/googletest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : google_tests/lib/googletest/CMakeFiles/gtest.dir/depend

