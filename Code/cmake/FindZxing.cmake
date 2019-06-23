# - Try to find the Zxing library

if(NOT ZXING_FOUND)
	FIND_PATH(ZXING_INCLUDE_DIR bigint/BigInteger.hh
	${PROJECT_SOURCE_DIR}/external/zxing/core/src	
	)
if(ZXING_INCLUDE_DIR)
set(ZXING_FOUND TRUE)

find_package(OpenCV REQUIRED)
set(LIBZXING_FILES ${ZXING_INCLUDE_DIR}/../../opencv/src/zxing)
message(STATUS ${LIBZXING_FILES})
set(ZXING_INCLUDE_DIRS ${ZXING_INCLUDE_DIR} ${LIBZXING_FILES} ${OPENCV_INCLUDE_DIRS} )
add_subdirectory(${ZXING_INCLUDE_DIR}/../..)
endif()
endif()
