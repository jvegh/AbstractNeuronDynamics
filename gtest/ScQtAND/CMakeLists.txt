# This is the CMakeLists file for testing the ScQtAND components
# The directory structure (and other docs) can be found in cmake/Docs
#
# @author János Végh

message(HIGHLIGHTED "                    Gtest tests")
#    find_package(SystemC REQUIRED)
#    find_package(GTest REQUIRED)


include_directories(
    include
    ../../neuron
    ../../neuron/neurons
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_SOURCE_DIR}/test/include
    ${GTEST_INCLUDE_DIRS}
    ${SystemC_INCLUDE_DIRS}
    )

file(GLOB_RECURSE TEST_INCLUDES
    include/*.h
)

link_directories(
    ${SystemC_LIBRARY_DIRS}
    ${NEURON_LIB}
 CONAN_PKG::gtest
 #   ${SCV_LIBRARY_DIRS}
)

set(TEST_SOURCES
    sctestbench_simple.cpp
#    TestAbstractPUBase.cpp
#    TestBioPUBase.cpp
#    TestTechPUBase.cpp
#   TestNeuronDemo.cpp
    TestNeuronPhysical.cpp
#    TestStuff.cpp
#    TestUtils.cpp
#    TestTopology.cpp
#    TestscGridPoint.cpp
#    TestRouting.cpp
#    TestHThread.cpp
)


ADD_EXECUTABLE(
    ${PROJECT_NAME}_GTEST
    ${PROJECT_NAME}_gtest.cpp	# Unit testing with gtest, no events
    ${TEST_SOURCES}
)

    target_compile_features(${PROJECT_NAME}_GTEST PRIVATE cxx_std_17)

target_link_libraries(
    ${PROJECT_NAME}_GTEST
    ${NEURON_LIB}
    ${SystemC_LIBRARIES}
    ${GTEST_LIBRARIES}
    pthread
)
link_directories(
    ${SystemC_LIBRARY_DIRS}
#    ${SCV_LIBRARY_DIRS}
)

install(TARGETS ${PROJECT_NAME}_GTEST
   RUNTIME
   DESTINATION bin
   COMPONENT tests
)

INSTALL(FILES ${TEST_SOURCES} CMakeLists.txt ${TEST_SOURCES} testbench_simple.cpp
        ${PROJECT_NAME}_gtest.cpp
	DESTINATION gtest
	COMPONENT srcs
)

INSTALL(FILES ${TEST_INCLUDES}
	DESTINATION gtest/include
	COMPONENT srcs
)


