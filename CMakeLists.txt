cmake_minimum_required (VERSION 2.6)

project (libpseudogpio)

add_library (
	pseudogpio
	SHARED

	src/libpseudogpio.h
	src/libpseudogpio.cpp
)

# pkg-config stuff
# Thanks to http://dailycommit.blogspot.com/2016/10/how-to-generate-pkg-config-file-with.html
set (DEST_DIR "${CMAKE_INSTALL_PREFIX}")
CONFIGURE_FILE ("libpseudogpio.pc.in" "libpseudogpio.pc" @ONLY)


install (TARGETS pseudogpio DESTINATION lib)
install (FILES src/libpseudogpio.h DESTINATION include)
install (FILES ${CMAKE_BINARY_DIR}/libpseudogpio.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")
