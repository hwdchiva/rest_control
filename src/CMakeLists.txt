#########################################################################################
#  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
# |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
# |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
# |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
# |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
# |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
#
#########################################################################################

cmake_minimum_required(VERSION 3.15.0)
project(rest_control)

cmake_policy(SET CMP0020 NEW)

#-----------------------------------------
# List the files 
#-----------------------------------------
file(GLOB rc_CPP_files *.cpp joystick/*.cpp)
file(GLOB rc_HEADER_files *.h joystick/*.h)
file(GLOB rc_FORM_files *.ui)

# Runs MOC on specifed files
QT5_WRAP_CPP( rc_MOC_files_cpp ${rc_HEADER_files} )

# Runs UIC on specified files
set(rc_FORM_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/GeneratedFiles)
SET(MKDIR_CMD "cmake" "-E" "make_directory" "${rc_FORM_OUTPUT_DIR}")
EXECUTE_PROCESS(
  COMMAND ${MKDIR_CMD}
)
QT5_WRAP_UI_OUTDIR( rc_FORM_headers ${rc_FORM_OUTPUT_DIR} ${rc_FORM_files})

# Runs RCC on specified files
QT5_ADD_RESOURCES ( rc_QRC_files ${rc_QRC_src_files} )

# Adds folders for Visual Studio solution explorer
source_group( "Generated Files" FILES ${rc_MOC_files_cpp} ${rc_FORM_headers} ${rc_QRC_files} )
source_group( "Form Files" FILES ${rc_FORM_files} )
source_group( "Resource Files" FILES ${rc_QRC_src_files} )

set(rc_sources ${rc_CPP_files} ${rc_HEADER_files} ${rc_MOC_files_cpp} ${rc_FORM_headers} ${rc_QRC_files} )

#-----------------------------------------
# Set up the target rc 
#-----------------------------------------
set(rc_include_dirs . ${rc_FORM_OUTPUT_DIR})
set(rc_include_dirs "${rc_include_dirs}" PARENT_SCOPE)
include_directories("${rc_include_dirs}")

add_executable(rest_control "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" ${rc_sources} ${CMAKE_CURRENT_SOURCE_DIR}/rc/CMakeSetupRestControl.rc)
target_link_libraries(rest_control ${WIN32_LIBS} ${QT_LIB_LIST} ${SDL_LIB})
set_target_properties(rest_control PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_target_properties(rest_control PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")

add_custom_command(TARGET rest_control PRE_BUILD COMMAND ${rc_version_cmd})
#add_custom_command(TARGET rest_control POST_BUILD COMMAND ${rc_windows10_manifest_cmd})


