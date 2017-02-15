# The recommended way to collect sources in variable 
# QUANTIZE_SOURCES by explicitly specifying the source files
set (QUANTIZE_SOURCES
	"Quantizer.h"
	"Quantizer.cpp"
	"main.cpp")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("" FILES ${QUANTIZE_SOURCES})

# Set Properties->General->Configuration Type to Application(.exe)
# Creates Huffman.exe with the listed sources collected in the variables
# Adds sources to the Solution Explorer
add_executable (Quantize ${QUANTIZE_SOURCES})

# Properties->Linker->Input->Additional Dependencies
target_link_libraries (Quantize boost_program_options OpenImageIO)

# Creates a folder "executables" and adds target 
# project (Quantize.vcproj) under it
set_property(TARGET Quantize PROPERTY FOLDER "executables")

# Adds logic to INSTALL.vcproj to copy Quantize.exe to destination directory
install (TARGETS Quantize
	RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin)
