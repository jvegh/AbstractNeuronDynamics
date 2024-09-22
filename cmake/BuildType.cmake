# Set the build type if it isn't done already
if(DEBUG_MODE)
  set(CMAKE_BUILD_TYPE Debug)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_RUNTIME_WORK_DIRECTORY "${CMAKE_BINARY_DIR}")
  message(HIGHLIGHTED "Configuring ${PROJECT_NAME} V${PROJECT_VERSION}@${PROJECT_TIMESTAMP} debug version")
else(DEBUG_MODE)
  add_definitions(-DNDEBUG)   # Disable asserts
  set(CMAKE_BUILD_TYPE Release)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_RUNTIME_WORK_DIRECTORY "${CMAKE_BINARY_DIR}")
  message(HIGHLIGHTED "Configuring ${PROJECT_NAME} V${PROJECT_VERSION}@${PROJECT_TIMESTAMP} release version")
  set(CMAKE_CXX_FLAGS "-DQT_NO_DEBUG_OUTPUT" )
endif(DEBUG_MODE)

# ---[ Flags
if(UNIX OR APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall")
endif()

if(USE_libstdcpp)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
  message("-- Warning: forcing libstdc++ (controlled by USE_libstdcpp option in cmake)")
endif()



