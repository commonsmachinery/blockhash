# Header
AC_PREREQ(2.60)
AC_INIT([blockhash], [0.2.0], [dev@commonsmachinery.se])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])

# Check programs
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

AC_SUBST([LIBTOOLS_DEPS])

INCLUDES="-I/usr/local/include"
DEFS="-Dcimg_verbosity=0 -Dcimg_display=0 -Dcimg_use_png -Dcimg_use_jpeg -Dcimg_use_tiff -Dcimg_use_openexr -Dcimg_use_zlib"
CFLAGS="$CFLAGS $DEFS $INCLUDES -Wall -Werror -fno-common -fmax-errors=3 -std=gnu99"
CXXFLAGS="$CXXFLAGS $DEFS $INCLUDES -Wall -Werror -fno-common -fmax-errors=3 -std=gnu++11 `pkg-config --cflags OpenEXR`"
CPPFLAGS="$CPPFLAGS $DEFS $INCLUDES `pkg-config --cflags OpenEXR`"
LDFLAGS="$LDFLAGS -L/usr/local/lib `pkg-config --libs OpenEXR` -lm"


# Checks for header files.
AC_CHECK_HEADER([machine/endian.h])
AC_CHECK_HEADER(png.h)
AC_CHECK_HEADER(jpeg.h)
AC_CHECK_HEADER(tiff.h)
AC_CHECK_HEADER(zlib.h)

dnl
dnl check openexr
dnl
AM_PATH_PKGCONFIG(
   [OPENEXR_CXXFLAGS],
   [OPENEXR_LDFLAGS],
   [OPENEXR_LIBS],
   [OpenEXR],
   [OpenEXR],
   [$LIB64_IF_EXISTS -L/usr/local/lib],
   [-lIlmImf -lImath -lIex -lHalf -lz],
   [openexr-prefix])
   
AC_MSG_CHECKING([whether libjpeg is present])
AC_CHECK_LIB([jpeg], [jpeg_destroy_decompress], [], [AC_MSG_ERROR([
*** libjpeg not found.
You need libjpeg. Refer to README.md for installation hints.])])

AC_MSG_CHECKING([whether libpng is present])
AC_CHECK_LIB([png], [png_create_read_struct], [], [AC_MSG_ERROR([
*** libpng not found.
You need libpng. Refer to README.md for installation hints.])])

AC_MSG_CHECKING([whether libtiff is present])
AC_CHECK_LIB([tiff], [TIFFOpen], [], [AC_MSG_ERROR([
*** libtiff not found.
You need libtiff. Refer to README.md for installation hints.])])

AC_MSG_CHECKING([whether FFmpeg is present])
AC_CHECK_LIB([avcodec], [avcodec_alloc_frame], [], [AC_MSG_ERROR([
*** libavcodec not found.
You need FFmpeg. Refer to README.md for installation hints.])])

AC_CHECK_LIB([avutil], [av_log_set_level], [], [AC_MSG_ERROR([
*** libavutil not found.
You need FFmpeg. Refer to README.md for installation hints.])])

AC_CHECK_LIB([avformat], [av_read_frame], [], [AC_MSG_ERROR([
*** libavformat not found.
You need FFmpeg. Refer to README.md for installation hints.])])

AC_CHECK_LIB([swscale], [sws_getContext], [], [AC_MSG_ERROR([
*** libswscale not found.
You need FFmpeg. Refer to README.md for installation hints.])])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T
AC_CHECK_TYPE([uint64_t],[],[AC_MSG_ERROR([*** Required type uint64_t not found.])])

# Generate files
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
