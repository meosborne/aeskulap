# declare project
PROJECT(dcmimgle)

# declare include directories which hold for all subdirectories
INCLUDE_DIRECTORIES(${dcmimgle_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${ZLIB_INCDIR})

# recurse into subdirectories
SUBDIRS(libsrc apps tests include/dcmtk/dcmimgle)
