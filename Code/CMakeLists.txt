# project name
PROJECT(CarvingQR)
# requirement of cmake version
cmake_minimum_required(VERSION 3.5)

# Suppress MSVC CRT warnings.
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    #add_definitions(/Za)
    add_definitions(/MP) # multi-core builds
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#Add include
set( INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include )
file(GLOB INCLUDE_DIRS ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.cpp )
add_library(core ${INCLUDE_DIRS})

find_package(ZXING REQUIRED)

#Add QR generator
set(QR_DIR ${PROJECT_SOURCE_DIR}/external/qrgen/qrgen)
add_subdirectory(${QR_DIR} qrgen.out)

#Add Boost
find_package(Boost REQUIRED )
if(NOT Boost_FOUND)
    message( FATAL_ERROR "Not found Boost")
endif()

#Add Cgal
find_package(CGAL REQUIRED COMPONENTS Core)
include(${CGAL_USE_FILE})
target_include_directories(core PUBLIC ${INCLUDE_DIR} ${ZXING_INCLUDE_DIRS} ${QR_DIR} ${Boost_INCLUDE_DIRS} ${CGAL_3RD_PARTY_INCLUDE_DIRS} ${CGAL_INCLUDE_DIRS}
 )
target_link_libraries(core libzxing  qrgen ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} ${CGAL_LIBRARIES})

# Add your project files
file(GLOB SRCFILES *.cpp)
add_executable(${PROJECT_NAME}_bin ${SRCFILES} )
#target_include_directories(${PROJECT_NAME}_bin PUBLIC ${INCLUDE_DIR} )
target_link_libraries(${PROJECT_NAME}_bin  core)

