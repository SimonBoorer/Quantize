cmake_minimum_required (VERSION 3.1.0 FATAL_ERROR)

# Maps to Visual Studio solution file (Quantize.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (Quantize)

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_STANDARD 11)

if(MSVC)
	# Set compiler flags and options. 
	# Here it is setting the Visual Studio warning level to 4
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})

# Sub-directories where more CMakeLists.txt exist
add_subdirectory(Quantize)
