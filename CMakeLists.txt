#########################################################
#    Copyright (C) 2015 Inpur Inc.
#  
#    All rights reserved.
#########################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)
PROJECT(facePro)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR}/include)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -lpthread  -g -ggdb -lpcre ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -m64")
SET(CMAKE_BUILD_TYPE DEBUG)

FIND_PACKAGE(OpenCV REQUIRED)

AUX_SOURCE_DIRECTORY(. DIR_LIST)
AUX_SOURCE_DIRECTORY(./src SRC_LIST)
LIST(APPEND DIR_LIST ${SRC_LIST})

#ADD_LIBRARY(feature SHARED  ${FEATURE_SRCS} )


ADD_EXECUTABLE(facePro ${DIR_LIST})
#TARGET_LINK_LIBRARIES(facePro ${Opencv_LIBS} )
target_link_libraries(facePro ${OpenCV_LIBS} )
SET(STASM_DIR  ${PROJECT_SOURCE_DIR}/lib)
#SET(STASM_DIR  ${PROJECT_SOURCE_DIR}/depends/stasm/build)
target_link_libraries(facePro ${STASM_DIR}/libstasm.a)
MESSAGE(STATUS "OPENCV_LIBS= " ${OpenCV_LIBS})
MESSAGE(STATUS "This is BINARY dir " ${PROJECT_BINARY_DIR})
                                              
