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
include google_tests/CMakeFiles/google_tests_run.dir/depend.make

# Include the progress variables for this target.
include google_tests/CMakeFiles/google_tests_run.dir/progress.make

# Include the compile flags for this target's objects.
include google_tests/CMakeFiles/google_tests_run.dir/flags.make

google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.o: google_tests/CMakeFiles/google_tests_run.dir/flags.make
google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.o: ../google_tests/simple_tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.o"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/google_tests_run.dir/simple_tests.cpp.o -c /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/simple_tests.cpp

google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/google_tests_run.dir/simple_tests.cpp.i"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/simple_tests.cpp > CMakeFiles/google_tests_run.dir/simple_tests.cpp.i

google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/google_tests_run.dir/simple_tests.cpp.s"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/simple_tests.cpp -o CMakeFiles/google_tests_run.dir/simple_tests.cpp.s

google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.o: google_tests/CMakeFiles/google_tests_run.dir/flags.make
google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.o: ../google_tests/cache_tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.o"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/google_tests_run.dir/cache_tests.cpp.o -c /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/cache_tests.cpp

google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/google_tests_run.dir/cache_tests.cpp.i"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/cache_tests.cpp > CMakeFiles/google_tests_run.dir/cache_tests.cpp.i

google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/google_tests_run.dir/cache_tests.cpp.s"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/shevchenya/CLionProjects/CourseWorkCache/google_tests/cache_tests.cpp -o CMakeFiles/google_tests_run.dir/cache_tests.cpp.s

# Object files for target google_tests_run
google_tests_run_OBJECTS = \
"CMakeFiles/google_tests_run.dir/simple_tests.cpp.o" \
"CMakeFiles/google_tests_run.dir/cache_tests.cpp.o"

# External object files for target google_tests_run
google_tests_run_EXTERNAL_OBJECTS =

google_tests/google_tests_run: google_tests/CMakeFiles/google_tests_run.dir/simple_tests.cpp.o
google_tests/google_tests_run: google_tests/CMakeFiles/google_tests_run.dir/cache_tests.cpp.o
google_tests/google_tests_run: google_tests/CMakeFiles/google_tests_run.dir/build.make
google_tests/google_tests_run: lib/libgtestd.a
google_tests/google_tests_run: lib/libgtest_maind.a
google_tests/google_tests_run: lib/libgtestd.a
google_tests/google_tests_run: google_tests/CMakeFiles/google_tests_run.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable google_tests_run"
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/google_tests_run.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
google_tests/CMakeFiles/google_tests_run.dir/build: google_tests/google_tests_run

.PHONY : google_tests/CMakeFiles/google_tests_run.dir/build

google_tests/CMakeFiles/google_tests_run.dir/clean:
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests && $(CMAKE_COMMAND) -P CMakeFiles/google_tests_run.dir/cmake_clean.cmake
.PHONY : google_tests/CMakeFiles/google_tests_run.dir/clean

google_tests/CMakeFiles/google_tests_run.dir/depend:
	cd /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/shevchenya/CLionProjects/CourseWorkCache /home/shevchenya/CLionProjects/CourseWorkCache/google_tests /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests /home/shevchenya/CLionProjects/CourseWorkCache/cmake-build-debug/google_tests/CMakeFiles/google_tests_run.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : google_tests/CMakeFiles/google_tests_run.dir/depend

