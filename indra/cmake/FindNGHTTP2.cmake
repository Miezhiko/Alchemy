FIND_PATH(nghttp2_INCLUDE_DIR nghttp2.h
/usr/local/include/nghttp2
/usr/local/include
/usr/include/nghttp2
/usr/include
)

SET(nghttp2_NAMES ${nghttp2_NAMES} nghttp2)
FIND_LIBRARY(nghttp2_LIBRARY
  NAMES ${nghttp2_NAMES}
  PATHS /usr/lib /usr/lib64 /usr/local/lib
  )

IF (nghttp2_LIBRARY AND nghttp2_INCLUDE_DIR)
    set(NGHTTP2_INCLUDE_DIR ${nghttp2_INCLUDE_DIR})
ELSE ()
  MESSAGE(FATAL_ERROR "Could not find nghttp2")
ENDIF (nghttp2_LIBRARY AND nghttp2_INCLUDE_DIR)
