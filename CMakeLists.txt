### SETUP ###
cmake_minimum_required (VERSION 3.0)
project(Car_Control VERSION 0.1 LANGUAGES CXX)

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost 1.55 REQUIRED COMPONENTS python3 system thread log)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

### VARIABLES ###
## app info 
set(APP_PACKAGE_NAME CarControl-someip) 
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include/CarControl CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/cmake/CarControl-someip)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(ABSOLUTE_${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

## directory paths
set(PRJ_SRC_VAR src)
set(PRJ_SRC_GEN_VAR src-gen)
set(PRJ_SRC_GEN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PRJ_SRC_GEN_VAR}) # ./src-gen
set(PRJ_SRC_GEN_CORE_DIR ${PRJ_SRC_GEN_DIR}/core) # ./src-gen/core
set(PRJ_SRC_GEN_SOMEIP_DIR ${PRJ_SRC_GEN_DIR}/someip) # ./src-gen/someip
set(CORE_RUNTIME_DIR ~/capicxx-core-runtime)
set(SOMEIP_RUNTIME_DIR ~/capicxx-someip-runtime)
set(VSOMEIP_DIR ~/vsomeip)
set(COMMONAPI_INTERFACE_VER v0/commonapi)

# all .cpp files in src-gen/someip/v0/commonapi
FILE(GLOB PRJ_SOMEIP_LIB_SRCS ${PRJ_SRC_GEN_SOMEIP_DIR}/${COMMONAPI_INTERFACE_VER}/*.cpp)

### DIRECTORIES ###
include_directories(
    #./include
    ${CMAKE_CURRENT_SOURCE_DIR}/include 	
    #./src-gen/core
    ${PRJ_SRC_GEN_CORE_DIR}     
    #./src-gen/someip
    ${PRJ_SRC_GEN_SOMEIP_DIR}
    ${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)
link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
    ${Boost_LIBRARY_DIR}
)

### BUILD ###
## Service
add_executable(CarControl
    ${PRJ_SRC_VAR}/GamePad.cpp
    ${PRJ_SRC_VAR}/PiRacer.cpp
    ${PRJ_SRC_VAR}/CarControlService.cpp
    ${PRJ_SRC_VAR}/CarControlStubImpl.cpp
)
target_link_libraries(CarControl
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python
    Boost::python
)
# Libaries
add_library(CarControl-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarControl-someip
    CommonAPI
    CommonAPI-SomeIP
)
set_target_properties(CarControl-someip PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION} SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})
target_include_directories(CarControl-someip PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/someip>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
)
set_target_properties (CarControl-someip PROPERTIES INTERFACE_LINK_LIBRARY "")


### INSTALL ###
file(GLOB_RECURSE HEADER_FILES "./src-gen/core/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*Proxy*.hpp")
foreach(HEADER ${HEADER_FILES})
    # Get absolute path of header files.
    get_filename_component(HEADER_ABS_PATH ${HEADER} ABSOLUTE)
    # Get relative path of header files.
    file(RELATIVE_PATH HEADER_REL_PATH ${PRJ_SRC_GEN_CORE_DIR} ${HEADER_ABS_PATH})
    # Get directory name from file name.
    get_filename_component(HEADER_DIR ${HEADER_REL_PATH} DIRECTORY)
    # Install headers to desired directory.
    install(FILES ${HEADER_ABS_PATH} DESTINATION ${INSTALL_INCLUDE_DIR}/${HEADER_DIR})
endforeach()

install(TARGETS CarControl
        EXPORT CarControlTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)

install(TARGETS CarControl-someip
        EXPORT CarControl-someipTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)

# Add all targets to the build-tree export set
export(TARGETS CarControl-someip
  FILE "${PROJECT_BINARY_DIR}/CarControl-someipTargets.cmake"
)

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE CarControl-someip)

# Create the CarControl-someipConfig.cmake and CarControl-someipConfigVersion files ...
file(RELATIVE_PATH REL_INCLUDE_DIR "${ABSOLUTE_INSTALL_CMAKE_DIR}" "${ABSOLUTE_INSTALL_INCLUDE_DIR}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarControl-someipConfig.cmake.in
  "${PROJECT_BINARY_DIR}/CarControl-someipConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarControl-someipConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/CarControl-someipConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/CarControl-someipConfig.cmake"
  "${PROJECT_BINARY_DIR}/CarControl-someipConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

install(EXPORT CarControl-someipTargets
  DESTINATION "${INSTALL_CMAKE_DIR}"
)

### Client 
# add_executable(CarControlClient
#     ${PRJ_SRC_VAR}/CarControlClient.cpp
#     ${PRJ_SRC_GEN_SOMEIP_DIR}/${COMMONAPI_INTERFACE_VER}/CarControlSomeIPProxy.cpp
#     ${PRJ_SRC_GEN_SOMEIP_DIR}/${COMMONAPI_INTERFACE_VER}/CarControlSomeIPDeployment.cpp
# )
## Libaries
# target_link_libraries(CarControlClient 
#     CommonAPI 
#     CommonAPI-SomeIP 
#     vsomeip3
# )