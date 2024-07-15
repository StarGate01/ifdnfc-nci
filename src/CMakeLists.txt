find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNFC_NCI REQUIRED libnfc-nci)
pkg_check_modules(PCSCLITE REQUIRED libpcsclite)

add_library(idfnfc-nci SHARED ifdnfc-nci.c atr.c)

include_directories(${LIBNFC_NCI_INCLUDE_DIRS} ${PCSCLITE_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")

target_link_libraries(ifdnfc-nci ${LIBNFC_NCI_LIBRARIES})
target_compile_options(ifdnfc-nci PRIVATE ${LIBNFC_NCI_CFLAGS_OTHER})

target_include_directories(ifdnfc-nci PUBLIC ${PROJECT_SOURCE_DIR}/src)

install(TARGETS ifdnfc-nci LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
