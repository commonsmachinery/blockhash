# Header
AC_PREREQ(2.60)
AC_INIT([blockhash], [0.2.0], [dev@commonsmachinery.se])
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])

CFLAGS="$CFLAGS -I/usr/local/include -Wall -Werror -fno-common -fmax-errors=3"
LDFLAGS="$LDFLAGS -L/usr/local/lib -lm"
CPPFLAGS="$CPPFLAGS -I/usr/local/include"

# Check programs
AC_PROG_CC
AM_PROG_AR
AC_DISABLE_SHARED
AC_PROG_LIBTOOL
#m4_pattern_allow([AM_PROG_AR], [AM_PROG_AR])

AC_SUBST([LIBTOOLS_DEPS])

# Checks for header files.
AC_CHECK_HEADER([machine/endian.h])

# Check for ImageMagic
AC_MSG_CHECKING([whether MagickWand is present])
CFLAGS+=" `MagickWand-config --cflags`"
CPPFLAGS+=" `MagickWand-config --cppflags`"
LDFLAGS+=" `MagickWand-config --ldflags`"
AC_CHECK_HEADER(wand/magick-wand.h)
AC_CHECK_LIB([MagickWand], [MagickWandGenesis], [], [AC_MSG_ERROR([
*** libmagickwand not found.
You need libmagickwand. Refer to README.md for installation hints.])])


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
AC_CONFIG_FILES([Makefile blockhash.pc src/Makefile])
AC_OUTPUT
