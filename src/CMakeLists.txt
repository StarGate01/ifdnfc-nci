find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNFC_NCI REQUIRED libnfc-nci)
pkg_check_modules(PCSCLITE REQUIRED libpcsclite)

add_library(idfnfc-nci SHARED ifdnfc-nci.c)

include_directories(${LIBNFC_NCI_INCLUDE_DIRS} ${PCSCLITE_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
SET(CMAKE_C_FLAGS_DEBUG "-g -O0")

target_link_libraries(idfnfc-nci ${LIBNFC_NCI_LIBRARIES} ${PCSCLITE_LIBRARIES})
target_compile_options(idfnfc-nci PRIVATE ${LIBNFC_NCI_CFLAGS_OTHER} ${PCSCLITE_CFLAGS_OTHER})

target_include_directories(idfnfc-nci PUBLIC ${PROJECT_SOURCE_DIR}/src)

install(TARGETS idfnfc-nci LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/idfnfc-nci.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
