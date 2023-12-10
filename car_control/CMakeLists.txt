### SETUP ###
cmake_minimum_required (VERSION 3.0)
set(PROJ_NAME Car_Control VERSION 0.1 LANGUAGES CXX)
project(${PROJ_NAME})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")

### PACKAGES ###
find_package(Python3 COMPONENTS Development)
find_package(Boost 1.55 REQUIRED COMPONENTS python3 system thread log)
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

### VARIABLES ###
set(PRJ_SRC_VAR src)
set(PRJ_SRC_GEN_VAR src-gen)
set(PRJ_SRC_GEN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${PRJ_SRC_GEN_VAR})
set(CORE_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/core)
set(SOMEIP_GEN_SRC_DIR ${PRJ_SRC_GEN_PATH}/someip)
set(CORE_RUNTIME_DIR ~/capicxx-core-runtime)
set(SOMEIP_RUNTIME_DIR ~/capicxx-someip-runtime)
set(VSOMEIP_DIR ~/vsomeip)
set(COMMONAPI_INTERFACE_VER v0/commonapi)

FILE(GLOB PRJ_SOMEIP_LIB_SRCS ./src-gen/someip/v0/commonapi/*.cpp)

### DIRECTORIES ###
include_directories(
	./include
    ./src-gen/core
    ./src-gen/someip
	${Python3_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)

link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_SOMEIP_CMAKE_DIR}/build
    ${Boost_LIBRARY_DIR}
    # ${SOMEIP_RUNTIME_DIR}/build
    # ${CORE_RUNTIME_DIR}/build
    # ${VSOMEIP_DIR}/build
)

### BUILD ###
# Services
add_executable(CarControlService
    ${PRJ_SRC_VAR}/GamePad.cpp
    ${PRJ_SRC_VAR}/PiRacer.cpp
    ${PRJ_SRC_VAR}/CarControlService.cpp
    ${PRJ_SRC_VAR}/CarControlStubImpl.cpp
)
target_link_libraries(CarControlService
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
    Python3::Python
    Boost::python
)
# Client 
add_executable(CarControlClient
    ${PRJ_SRC_VAR}/CarControlClient.cpp
    ${SOMEIP_GEN_SRC_DIR}/${COMMONAPI_INTERFACE_VER}/CarControlSomeIPProxy.cpp
    ${SOMEIP_GEN_SRC_DIR}/${COMMONAPI_INTERFACE_VER}/CarControlSomeIPDeployment.cpp
)
target_link_libraries(CarControlClient 
    CommonAPI 
    CommonAPI-SomeIP 
    vsomeip3
)

add_library(CarControlService-someip
    SHARED ${PRJ_SOMEIP_LIB_SRCS}
)
target_link_libraries(CarControlService-someip
    CommonAPI
    CommonAPI-SomeIP
)