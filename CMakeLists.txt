##############################################################
#
#  This is the major CMake configuration file for the SystemC
#  TimeAwareSimulation project.
#  To use it you need CMake which can be
#  downloaded from here:
#    http://www.cmake.org/cmake/resources/software.html
#
# The directory structure (and other docs) can be found in cmake/Docs
##############################################################
#
# @author János Végh
#[ --- These are mandatory introduction offset
cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake
                      ${CMAKE_CURRENT_SOURCE_DIR}/..cmake  ${SYSTEMC_HOME})
#[ --- Set the name and version of your project here
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0007 NEW)
#cmake_policy(SET CMP0084 NEW)
include(Colorize)   # Just to make some beauty
    set(CPACK_PACKAGE_NAME "ScQtAND")
    set(MyVersion "0.0.6")
    set(MyDate "2024.09.20")
    set(Description "A graphical time-aware neuron simulator based on simulator RIPES")
project(ScQtAND VERSION ${MyVersion}
        LANGUAGES C CXX
        DESCRIPTION Description
        HOMEPAGE_URL "https://jvegh.github.io/AbstractNeuronDynamics"
)
include(BuildType)
include(Summary)

# ---[ Set up CMake options for the simulator
# Choose if clocked physical bus is to be used; slows down simulation; not yet in use
  option (USE_CLOCKED_BUS "Simulate clocked physical bus" OFF)
# Choose whether you need debugging support for development or making a release
  option (DEBUG_MODE "Include debug support for the package"  ON)
# Choose whether the stand-alone unit testing is to be built
  option (BUILD_TESTS "Include unit tests for the package"  ON)
# Choose whether to make documention as well
  option (BUILD_DOCS "Prepare also HTML/CHM documentation" ON)
# Choose whether you need internal information to docs
  option (DOXY_INTERNAL "Include internal info in the docs"  YES)
# ---[ End up CMake options for the user
message(HIGHLIGHTED ${Description})

if(NOT DEFINED ENV{QT_HOST_PATH})
       set(CMAKE_PREFIX_PATH "~/Qt6/6.6.1/gcc_64")
       message(HIGHLIGHTED "QT_HOST_PATH not defined; QT_HOST_PATH= ${CMAKE_PREFIX_PATH}")
else()
       message(HIGHLIGHTED "QT_HOST_PATH = ${QT_HOST_PATH}")
endif()

if(NOT DEFINED ENV{SYSTEMC_HOME})
       message(HIGHLIGHTED "SYSTEMC_HOME not defined; use")
       set(SystemC_INCLUDE_DIRS "/opt/systemc/include")
       set(SystemC_LIBRARY_DIRS "/opt/systemc/lib")
       message(HIGHLIGHTED "    SystemC_INCLUDE_DIRS = ${SystemC_INCLUDE_DIRS}")
       message(HIGHLIGHTED "    SystemC_LIBRARY_DIRS = ${SystemC_LIBRARY_DIRS}")
else()
       message(HIGHLIGHTED "SYSTEMC_HOME = ${SYSTEMC_HOME}")
endif()

# ---[ Do not edit below this line until you know what you are doing
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "CCache enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()
# ---[ Set up CMake options for multiplatform compilation

# ---[ Error flags on everything but MSVC
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    include(GNUInstallDirs) #Must exist after declaring a project
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wextra -Wall \
        -Werror=switch -Werror=return-type \
        -Werror=unreachable-code")
endif()

if(MSVC)
    add_definitions(/bigobj) # Allow big object
elseif(MINGW)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbig-obj") # Allow big object
endif()
# ---[ End up CMake options  for multiplatform compilation

# ---[ Install SystemC
if(NOT DEFINED ENV{SYSTEMC_HOME})

	find_program (CONAN_EXE NAMES conan REQUIRED)

	if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
	  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
	  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
	                "${CMAKE_BINARY_DIR}/conan.cmake"
	                TLS_VERIFY ON)
	endif()

	include(${CMAKE_BINARY_DIR}/conan.cmake)

	conan_add_remote(NAME minres URL https://git.minres.com/api/packages/Tooling/conan)

	conan_cmake_configure(REQUIRES systemc/2.3.3 #systemc-cci/1.0.0 systemc-scv/2.0.1
	                      #OPTIONS systemc-cci:shared=False
	                      SETTINGS compiler.cppstd=${CMAKE_CXX_STANDARD}
	                      GENERATORS cmake_find_package)

	conan_cmake_autodetect(settings)

	conan_cmake_install(PATH_OR_REFERENCE .
	                    BUILD missing
	                    SETTINGS ${settings} compiler.cppstd=${CMAKE_CXX_STANDARD})

	find_package(SystemCLanguage)
#	find_package(systemc-scv)
#	find_package(systemc-cci)
else()
	find_package(SystemC REQUIRED)
endif()
message(HIGHLIGHTED "SystemC installed")
include_directories(${SystemC_INCLUDE_DIRS})


# ---[ Install GTest
#if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    add_subdirectory(gtest)
    message(HIGHLIGHTED "Testing with GTest installed")
#endif()



######################################################################
## GUI setup
######################################################################

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(0)
# ---[ Set up RIPES
set(RIPES_WITH_QPROCESS ON CACHE BOOL "Build with QProcess-related utilities")
# Add as a C++ definition
if(RIPES_WITH_QPROCESS)
    add_definitions(-DRIPES_WITH_QPROCESS)
endif()
endif(0)

# Find required Qt packages
find_package(Qt6 COMPONENTS Core Widgets Svg Charts REQUIRED)

# Finding Qt includes
include_directories(${Qt6Widgets_INCLUDE_DIRS})
include_directories(${Qt6Core_INCLUDE_DIRS})
include_directories(${Qt6Charts_INCLUDE_DIRS})

######################################################################
## Resources setup
######################################################################
qt6_add_resources(ICONS_SRC ${CMAKE_SOURCE_DIR}/resources/icons/icons.qrc)
qt6_add_resources(EXAMPLES_SRC ${CMAKE_SOURCE_DIR}/examples/examples.qrc)
qt6_add_resources(LAYOUTS_SRC ${CMAKE_SOURCE_DIR}/src/processors/layouts.qrc)
qt6_add_resources(FONTS_SRC ${CMAKE_SOURCE_DIR}/resources/fonts/fonts.qrc)

if(0)
######################################################################
## Library setup
######################################################################

# Header-only libraries used in the project must be included from top CMakeLists.txt file
include_directories(${RIPES_LIB} SYSTEM PUBLIC external/VSRTL/external/cereal/include)
include_directories(${RIPES_LIB} PUBLIC external/VSRTL/external)
include_directories(SYSTEM external/ELFIO)
include_directories(SYSTEM external/VSRTL/external/Signals)
include_directories(external/libelfin_cmake)
include_directories(external)
endif(0)

if(0)
option(RIPES_BUILD_VERILATOR_PROCESSORS "Build verilator processors" OFF)
if(RIPES_BUILD_VERILATOR_PROCESSORS)
    if(NOT DEFINED ENV{VERILATOR_ROOT})
        message(FATAL_ERROR "'VERILATOR_ROOT' must be set when building Verilator-based processors!")
    endif()
    message(STATUS "Building with Verilator processors")
    include_directories(SYSTEM $ENV{VERILATOR_ROOT}/include)
    set(verilator_DIR $ENV{VERILATOR_ROOT})
    find_package(verilator)

    # Create library containing Verilator runtime files, which verilator processors can link against
    file(GLOB VERILATOR_SOURCES $ENV{VERILATOR_ROOT}/include/verilated.cpp)
    file(GLOB VERILATOR_HEADERS $ENV{VERILATOR_ROOT}/include/verilated.h)
    set(VERILATOR_LIB verilator_lib)
    add_library(${VERILATOR_LIB} ${VERILATOR_SOURCES} ${VERILATOR_HEADERS})
endif()
endif(0)

# Platform specific pre-target setup(s)
if(${APPLE})
    set(SYSTEM_FLAGS MACOSX_BUNDLE)
elseif(${WIN32})
    set(SYSTEM_FLAGS WIN32)
endif()

set(NEURON_LIB neuron_lib)

if(0)
add_subdirectory(external)

# Fix the name of the ripes library.
set(RIPES_LIB ripes_lib)
set(ISA_LIB isa_lib)
add_subdirectory(src)
endif(0)

add_subdirectory(neuron)

add_subdirectory(demos)


if(BUILD_DOCS)
  find_package(Doxygen)
  add_subdirectory(doc) # The doxygen documentation
  set(doxyfile_in "${CMAKE_SOURCE_DIR}/doc/Doxyfile.in")
endif(BUILD_DOCS)

if(0)
set(APP_NAME NeuRipes)
qt_add_executable(${APP_NAME} ${SYSTEM_FLAGS} ${ICONS_SRC} ${EXAMPLES_SRC} ${LAYOUTS_SRC} ${FONTS_SRC} main.cpp)

# Link Qt libraries
target_link_libraries(${APP_NAME} PUBLIC Qt6::Core Qt6::Widgets)
# Link Ripes library
target_link_libraries(${APP_NAME} PUBLIC ${RIPES_LIB} ${NEURON_LIB} SystemC::systemc)
endif(0)

if(0)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    # https://doc.qt.io/qt-6/wasm.html#asyncify
    target_link_options(${RIPES_LIB} PUBLIC -sASYNCIFY -Os)
    target_link_options(${APP_NAME} PUBLIC -sASYNCIFY -Os)
endif()
endif(0)


if(0)
if(UNIX AND NOT APPLE) #Define the LINUX variable before testing it
    set(LINUX TRUE)
endif()

if(${LINUX})
    install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
elseif(${APPLE})
    install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE  DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/appdir/usr/
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)
endif(0)

caffe_print_configuration_summary()