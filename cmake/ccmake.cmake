#=========================================================================================
#  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
# |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
# |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
# |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
# |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
# |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
#
#=========================================================================================

macro (addQT5)
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    FIND_PACKAGE(Qt5Core REQUIRED)
    set(QT_MIN_VERSION 5.15.0)
    set(QT_MAX_VERSION 5.15.2)
    if(Qt5Core_VERSION VERSION_LESS QT_MIN_VERSION)
        MESSAGE(FATAL_ERROR "Minimum supported Qt version: ${QT_MIN_VERSION}. Installed version: ${Qt5Core_VERSION}")
    elseif(Qt5Core_VERSION VERSION_GREATER QT_MAX_VERSION)
        MESSAGE(FATAL_ERROR "Maximum supported Qt version: ${QT_MAX_VERSION}. Installed version: ${Qt5Core_VERSION}")
    endif()

    SET (QT5_REQUIRED_LIBS Qt5Core Qt5Charts Qt5Gui Qt5Xml Qt5Multimedia Qt5Network Qt5Test Qt5Widgets Qt5Svg Qt5Sql Qt5Concurrent Qt5Quick Qt5PrintSupport Qt5Positioning)
    FOREACH (qtlib ${QT5_REQUIRED_LIBS})
        # find QT libraries
        FIND_PACKAGE(${qtlib} REQUIRED)

        #include dirs
        INCLUDE_DIRECTORIES(${${qtlib}_INCLUDE_DIRS})

        # add definitions
        ADD_DEFINITIONS(${${qtlib}_DEFINITIONS})

        # add libraries
        list(APPEND QT_LIB_LIST ${${qtlib}_LIBRARIES})
    ENDFOREACH (qtlib)
endmacro()

# ============= setPlatformInfo  macro =========
macro (setPlatformInfo)
    include(CMakeParseArguments)
    if (WIN32)
        message(STATUS "MSVC_VERSION = " ${MSVC_VERSION})
        message(STATUS "MSVC_CXX_ARCHITECTURE_ID = ${MSVC_CXX_ARCHITECTURE_ID}")
        message(STATUS "CMAKE_CL_64 = ${CMAKE_CL_64}")
        set(vs2019_versions "1920" "1921" "1922" "1923" "1924" "1925" "1926" "1927" "1928" "1933" "1939")

        if(MSVC_VERSION IN_LIST vs2019_versions) # VS2019
            if (CMAKE_CL_64) # Using the 64 bit compiler from Microsoft
                set (BUILD_TYPE windows_vs19_x64)
            else()
                set (BUILD_TYPE windows_vs19_x32)
                message(FATAL_ERROR "This Visual Studio version ${MSVC_VERSION} (MSVS2019) with 32-bit platform not supported")
            endif()
        else()
            message(FATAL_ERROR "This Visual Studio version ${MSVC_VERSION} is not supported.")
        endif()
    else()
        message ( FATAL_ERROR "Platform not supported." )
    endif()

    message(STATUS "[${PROJECT_NAME}]: detected platform: " ${BUILD_TYPE})

endmacro()

# QT5_WRAP_UI_OUTDIR(outfiles outdir inputfile ... )
MACRO (QT5_WRAP_UI_OUTDIR outfiles outdir )
  #QT4_EXTRACT_OPTIONS(ui_files ui_options ${ARGN})
  QT4_EXTRACT_OPTIONS_OUTDIR(ui_files ui_options ${ARGN})

  FOREACH (it ${ui_files})
    GET_FILENAME_COMPONENT(outfile ${it} NAME_WE)
    GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
    #SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/ui_${outfile}.h) # Here we set output
    SET(outfile ${outdir}/ui_${outfile}.h) # Here we set output
    ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
      COMMAND ${Qt5Widgets_UIC_EXECUTABLE}
      ARGS ${ui_options} -o ${outfile} ${infile}
      MAIN_DEPENDENCY ${infile})
    SET(${outfiles} ${${outfiles}} ${outfile})
  ENDFOREACH (it)

ENDMACRO (QT5_WRAP_UI_OUTDIR)

MACRO (QT4_EXTRACT_OPTIONS_OUTDIR _qt4_files _qt4_options)
  SET(${_qt4_files})
  SET(${_qt4_options})
  SET(_QT4_DOING_OPTIONS FALSE)
  FOREACH(_currentArg ${ARGN})
    IF ("${_currentArg}" STREQUAL "OPTIONS")
      SET(_QT4_DOING_OPTIONS TRUE)
    ELSE ("${_currentArg}" STREQUAL "OPTIONS")
      IF(_QT4_DOING_OPTIONS) 
        LIST(APPEND ${_qt4_options} "${_currentArg}")
      ELSE(_QT4_DOING_OPTIONS)
        LIST(APPEND ${_qt4_files} "${_currentArg}")
      ENDIF(_QT4_DOING_OPTIONS)
    ENDIF ("${_currentArg}" STREQUAL "OPTIONS")
  ENDFOREACH(_currentArg) 
ENDMACRO (QT4_EXTRACT_OPTIONS_OUTDIR)