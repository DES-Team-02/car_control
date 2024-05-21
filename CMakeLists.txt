### SETUP ###
cmake_minimum_required (VERSION 3.15)
project(Car_Control VERSION 0.1 LANGUAGES CXX)

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost 1.55 REQUIRED COMPONENTS python3 system thread log)
#find_package(Boost REQUIRED COMPONENTS python3)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

### VARIABLES ###
## Application Information ## 
set(APP_PACKAGE_NAME commonapi) 
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
## Compiler Flags ##
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
## Install Directories ##
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include/CarControl CACHE PATH "Installation directory for header files")
set(INSTALL_PIRACER_DIR lib/piracer CACHE PATH "Installation directory for piracer")
set(DEF_INSTALL_CMAKE_DIR lib/cmake/CarControl-someip)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")
foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(ABSOLUTE_${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
## Executable Sources ##
set(Car_Control_SRCS
    ./src/GamePad.cpp
    ./src/PiRacer.cpp
    ./src/CarControlService.cpp
    ./src/CarControlStubImpl.cpp
    ./src/CanProxy.cpp
    ./src/JetsonProxyImpl.cpp
    ./src/JoyStick.cpp
    ./src/ShawnGamePad.cpp
)
## Library Sources ##
FILE(GLOB PRJ_SOMEIP_LIB_SRCS ./src-gen/someip/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*.cpp)

### INCLUDE & LINK DIRECTORIES ###
include_directories(
    ./include	             
    ./src-gen/core               
    ./src-gen/someip        
    ${Boost_INCLUDE_DIRS}
    ${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
    ~/DES_4_CanTransceiver/src-gen/core
    ~/DES_4_pydbus_module/src-gen/core
)
link_directories(
    ${Boost_LIBRARY_DIR}
    ${Python3_LIBRARY_DIRS}
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
)

### BUILD TARGETS ###
## Executable ## 
add_executable(CarControl
    ${Car_Control_SRCS}
)
target_link_libraries(CarControl
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python
    Boost::python
)
## Libaries ##
add_library(CarControl-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarControl-someip
    CommonAPI
    CommonAPI-SomeIP
)
## Properties ##
set_target_properties(CarControl-someip PROPERTIES 
    VERSION ${MAJOR_VERSION}.${MINOR_VERSION} 
    SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION}
)
target_include_directories(CarControl-someip PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen/someip>
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
)
set_target_properties (CarControl-someip PROPERTIES 
    INTERFACE_LINK_LIBRARY ""
)

### INSTALLATION ###
## Piracer files ##
file(GLOB_RECURSE PIRACER_FILES "./piracer/*.py")
foreach(PIRACER_FILE ${PIRACER_FILES})
    get_filename_component(PIRACER_FILE_ABS_PATH ${PIRACER_FILE} ABSOLUTE)                         # Get absolute path of files.
    file(RELATIVE_PATH PIRACER_FILE_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/piracer ${PIRACER_FILE_ABS_PATH}) # Get relative path of files.
    get_filename_component(PIRACER_FILE_DIR ${PIRACER_FILE_REL_PATH} DIRECTORY)                    # Get directory name from file name.
    install(FILES ${PIRACER_FILE_ABS_PATH} DESTINATION ${INSTALL_PIRACER_DIR}/CarControl) # Install to desired directory.
endforeach()
## Headers ##
file(GLOB_RECURSE HEADER_FILES "./src-gen/core/v${MAJOR_VERSION}/${APP_PACKAGE_NAME}/*.hpp")
foreach(HEADER ${HEADER_FILES})
    get_filename_component(HEADER_ABS_PATH ${HEADER} ABSOLUTE)                         # Get absolute path of header files.
    file(RELATIVE_PATH HEADER_REL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src-gen/core ${HEADER_ABS_PATH})     # Get relative path of header files.
    get_filename_component(HEADER_DIR ${HEADER_REL_PATH} DIRECTORY)                    # Get directory name from file name.
    install(FILES ${HEADER_ABS_PATH} DESTINATION ${INSTALL_INCLUDE_DIR}/${HEADER_DIR}) # Install headers to desired directory.
endforeach()
## Targets ##
install(TARGETS CarControl
        EXPORT CarControlTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)
## Libraries ##
install(TARGETS CarControl-someip
        EXPORT CarControl-someipTargets
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
)

### EXPORT ###
## Targets ##
#(Add all targets to the build-tree export set)
export(TARGETS CarControl-someip
    FILE "${PROJECT_BINARY_DIR}/CarControl-someipTargets.cmake" 
)
#(Export the package for use from the build-tree(this registers the build-tree with a global CMake-registry))
export(PACKAGE CarControl-someip)
## Configuration files ##
#(Create the CarControl-someipConfig.cmake and CarControl-someipConfigVersion files ...)
file(RELATIVE_PATH REL_INCLUDE_DIR 
    "${ABSOLUTE_INSTALL_CMAKE_DIR}" "${ABSOLUTE_INSTALL_INCLUDE_DIR}"
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarControl-someipConfig.cmake.in
    "${PROJECT_BINARY_DIR}/CarControl-someipConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CarControl-someipConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/CarControl-someipConfigVersion.cmake" @ONLY
)
install(FILES
    "${PROJECT_BINARY_DIR}/CarControl-someipConfig.cmake"
    "${PROJECT_BINARY_DIR}/CarControl-someipConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}"
)
install(EXPORT CarControl-someipTargets
    DESTINATION "${INSTALL_CMAKE_DIR}"
)