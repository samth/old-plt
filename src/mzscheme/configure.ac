
#################################################################
# This is the source for the `configure' script, to be compiled #
# by autoconf.                                                  #
#################################################################

# Remember:
#  CFLAGS - C compilation only
#  CXXFLAGS - C++ compilation only
#  CPPFLAGS - C and C++ pre-processing and compilation for configure-time testing ONLY
#  COMPFLAGS - C and C++ compilation, but not configure-time tests
#  PREFLAGS - C and C++ preprocessing, but not configure-time tests
#  OPTIONS - C and C++ flags, not needed for configure-time tests
#  MZOPTIONS - MzScheme-only flags
#  MROPTIONS - MrEd-only flags
#  CGCOPTIONS - Boehm-GC-only flags
#  GC2OPTIONS - GC2-only flags
#
# Note: we use CPPFLAGS for configure-time testing only because
# it needs to include C++ flags that we don't want for MzScheme.
# hence PREFLAGS, which is initialized to the original CPPFLAGS.

AC_INIT([PLT Scheme])
AC_CONFIG_SRCDIR(mzscheme/src/bignum.c)
AC_CONFIG_HEADERS([mzscheme/mzconfig.h])

AC_ARG_ENABLE(mred, [  --enable-mred           compile MrEd as well as MzScheme (enabled by default)], , enable_mred=yes )

AC_ARG_ENABLE(gl, [  --enable-gl             use OpenGL when available (enabled by default)])
AC_ARG_ENABLE(xrender, [  --enable-xrender        use Xrender when available (enabled by default)])
AC_ARG_ENABLE(xft, [  --enable-xft            use Xft when available (enabled by default)])
AC_ARG_ENABLE(cairo, [  --enable-cairo          use Cairo when available (enabled by default)])
AC_ARG_ENABLE(libpng, [  --enable-libpng         use libpng instead of building (enabled by default)])
AC_ARG_ENABLE(libjpeg, [  --enable-libjpeg        use libjpeg instead of building (enabled by default)])

AC_ARG_ENABLE(shared, [  --enable-shared         create shared libraries])
AC_ARG_ENABLE(dynlib, [  --enable-dynlib         same as --enable-shared])

AC_ARG_ENABLE(foreign, [  --enable-foreign        compile foreign support (enabled by default)], , enable_foreign=yes)

AC_ARG_ENABLE(sgc, [  --enable-sgc            use Senora GC instead of the Boehm GC])
AC_ARG_ENABLE(sgcdebug, [  --enable-sgcdebug       use Senora GC for debugging])
AC_ARG_ENABLE(account, [  --enable-account        3m: use memory-accounting GC (enabled by default)], , enable_account=yes)
AC_ARG_ENABLE(compact, [  --enable-compact        3m: use compact GC (no accounting)])
AC_ARG_ENABLE(backtrace, [  --enable-backtrace      3m: compact plus backtrace info])

AC_ARG_ENABLE(float, [  --enable-float          include support for single-precision floats])
AC_ARG_ENABLE(floatinstead, [  --enable-floatinstead   compile to use single-precision by default])

AC_ARG_ENABLE(pthread, [  --enable-pthread        link MrEd with pthreads (sometimes needed for GL)])

AC_ARG_ENABLE(oskit, [  --enable-oskit          compile OSKit-based MzScheme kernel])
AC_ARG_ENABLE(smalloskit, [  --enable-smalloskit     compile small OSKit-based MzScheme kernel])

AC_ARG_ENABLE(wbuild, [  --enable-wbuild         compile and use wbuild for .w sources])
AC_ARG_ENABLE(perl, [  --enable-perl           use perl to regenerate certain sources])

AC_ARG_ENABLE(gprof, [  --enable-gprof          compile for profiling with gprof (gcc only)])
AC_ARG_ENABLE(gcov, [  --enable-gcov           compile to gather gcov statistics (gcc3 only)])

AC_ARG_ENABLE(noopt, [  --enable-noopt          drop -O C flags])

AC_ARG_ENABLE(xonx, [  --enable-xonx           compile X11 (not Quartz) MrEd for Mac OS X])
AC_ARG_ENABLE(libfw, [  --enable-libfw          install Mac OS X frameworks to /Library/Frameworks])

###### Get OS Type #######

if test -x "/bin/uname" ; then
  UNAME=/bin/uname
elif test -x "/usr/bin/uname" ; then
  UNAME=/usr/bin/uname
elif test -x "/usr/local/bin/uname" ; then
  UNAME=/usr/local/bin/uname
else
  echo configure: cannot find uname
  exit 1
fi

OS=`$UNAME -s`

###### Get data sizes #######

AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

###### Some flags imply other flags #######

if test "${enable_smalloskit}" = "yes" ; then
  enable_oskit=yes
fi

if test "${enable_oskit}" = "yes" ; then
  enable_mred=no
fi

if test "${enable_dynlib}" = "no" ; then
  enable_shared=no
fi

if test "${enable_mred}" = "no" ; then
  enable_wbuild=no
  enable_xrender=no
  enable_cairo=no
  enable_libpng=no
  enable_libjpeg=no
  enable_gl=no
  enable_pthread=no
fi

enable_quartz=no
if test "${enable_xonx}" = "yes" ; then
  enable_quartz=no
else
  if test "$OS" = "Darwin" ; then
    enable_quartz=yes
  fi
fi

# Hack for the implementor:
if test "${enable_perl}" = "" ; then
  if test "$USER" = "mflatt" ; then
    enable_perl=yes
  fi
  if test "$USER" = "matthewf" ; then
    enable_perl=yes
  fi
fi

if test "${prefix}" = "NONE" ; then
  prefix=`cd "${srcdir}/.." && pwd`
  MAKE_FINISH=inplace-finish
else
  # Check whether ${prefix} is redundant, because
  # $prefix/src is $srcdir.
  here_inode=`(ls -i -l -d ${srcdir}/. | cut -d d -f 1) 2> /dev/null`
  there_inode=`(ls -i -l -d ${prefix}/src | cut -d d -f 1) 2> /dev/null`
  if test "${here_inode}" = "${there_inode}" ; then
    echo "in-place install, --prefix  specification is redundant"
  else 
    echo "----> Installation will go to ${prefix}:"
    echo "---->   ${prefix}/collects/..."
    echo "---->   ${prefix}/bin/..."
    echo "---->   ${prefix}/lib/..."
    echo "---->   ${prefix}/include/..."
    echo "---->   ${prefix}/man/..."
    echo "---->   ${prefix}/notes/..."
    MAKE_COPYTREE=copytree
    MAKE_FINISH=copy-finish
  fi
fi

if test "${enable_iconv}" = "" ; then
  enable_iconv=yes
fi

###### Make sure MrEd is really there ######

if test "${enable_mred}" = "yes" ; then
  if test ! -d $srcdir/mred ; then
    enable_mred=no
  fi
fi

###### Some defaults #######

OPTIONS=

# GC Directory
GCDIR=gc

MZBINTARGET=normal-bin
MZINSTALLTARGET=normal-install

MZOPTIONS=
CGCOPTIONS=
GC2OPTIONS=

LIBS=

WXVARIANT="wx_xt"

MROPTIONS=
INCLUDEDEP="#"
WX_MMD_FLAG=

JPEG_A='$(JPEG_A)'
JPEG_INC='$(JPEG_INC)'

ZLIB_A='$(ZLIB_A)'
ZLIB_INC='$(ZLIB_INC)'

PNG_A='$(PNG_A)'

PREFLAGS="$CPPFLAGS"

###### OSKit stuff #######

if test "${enable_oskit}" = "yes" ; then
  OSKHOME=${OSKHOME:-/usr/local/oskit}

  if test ! -d ${OSKHOME}/bin ; then
    AC_MSG_ERROR(cannot find OSKit support; try defining OSKHOME)
  fi

  CC=`(ls ${OSKHOME}/bin/*-oskit-gcc | head -1) 2> /dev/null`
  if test ! -x "${CC}" ; then
    AC_MSG_ERROR(can't find *-oskit-gcc compiler script in ${OSKHOME}/bin)
  fi

  PREFLAGS="-nostdinc -I${OSKHOME}/include/oskit -I${OSKHOME}/include/oskit/freebsd "
  PREFLAGS="$PREFLAGS -I${OSKHOME}/include/oskit/c -I${OSKHOME}/include -I/usr/include"

  GCDIR=sgc
  OPTIONS="-DOSKIT -DUSE_SENORA_GC -DSGC_AUTO_ROOTS=0"

  if test "${enable_smalloskit}" = "yes" ; then
    EXTRA_OSK_LIBS=
  else
    EXTRA_OSK_LIBS="-loskit_linux_dev -loskit_linux_fs -loskit_diskpart -loskit_freebsd_net"
    OPTIONS="$OPTIONS -DOSK_FILESYSTEMS_AND_ETHERNET"
  fi

  MZBINTARGET=oskit
else
  enable_oskit=no
fi

###### float stuff #######

if test "${enable_floatinstead}" = "yes" ; then
  enable_float=yes
  OPTIONS="${OPTIONS} -DUSE_SINGLE_FLOATS_AS_DEFAULT"
fi

if test "${enable_float}" = "yes" ; then
  OPTIONS="${OPTIONS} -DUSE_SINGLE_FLOATS"
fi

###### Autoconfigure #######

COMPFLAGS=""

if test "${enable_oskit}" = "no" ; then
  AC_PROG_CC
  AC_PROG_CPP
  AC_PROG_CXX
  AC_PROG_CXXCPP
  if test "$AS" = '' ; then 
    AS=as
    as_was_set=no
  else
    as_was_set=yes
  fi
fi
AC_PROG_RANLIB
if test "$AR" = '' ; then 
  AR=ar
fi
if test "$ARFLAGS" = '' ; then 
  ARFLAGS=ruv
fi

# If using gcc, we want all warnings:
if test "$CC" = "gcc" ; then 
  COMPFLAGS="$COMPFLAGS -Wall"

  # Use -MMD when we have gcc and gnumake:
  is_gmake=`make -v no-such-target-we-hope 2>&1 | grep "GNU Make"`
  if test "$is_gmake" = "" ; then
    WX_MMD_FLAG=""
  else
    WX_MMD_FLAG="-MMD"
    INCLUDEDEP="-include"
  fi
fi

if test "${enable_perl}" = "yes" ; then
  AC_PATH_PROG(PERL,perl)
else
  PERL=""
fi

if test "$REZ" = '' ; then 
 REZ=/Developer/Tools/Rez
fi

if test "${enable_mred}" = "yes" ; then
  if test "${enable_quartz}" = "yes" ; then
   :
  else
   AC_PATH_XTRA
  fi

  MAKE_MRED=mr
  MAKE_MRED3M=mr3m
  MAKE_MREDINSTALL=mrinstall
  MAKE_MREDINSTALL3M=mrinstall3m
else
  MAKE_MRED=
  MAKE_MRED3M=
  MAKE_MREDINSTALL=
  MAKE_MREDINSTALL3M=
fi

AC_CHECK_LIB(m, cos)
AC_CHECK_LIB(dl, dlopen)

PROFFLAGS=""

if test "${enable_gprof}" = "yes" ; then
  MZOPTIONS="$MZOPTIONS -DDONT_ITIMER"
  PROFFLAGS="$PROFFLAGS -pg"
  LIBS="$LIBS -pg"
fi

if test "${enable_gcov}" = "yes" ; then
  PROFFLAGS="$PROFFLAGS -fprofile-arcs -ftest-coverage"
fi

if test "${enable_wbuild}" = "yes" ; then
  MAKE_WBUILD=wb
  WBUILD="x/wbuild/src/wbuild"
  WBUILD="$WBUILD --no-init-file --no-lines -i \$(srcdir)/XWidgets"
  WBUILD="$WBUILD -c \$(srcdir)/XWidgets -d \$(srcdir)/XWidgets"
  WBUILD="$WBUILD -p . \$(srcdir)/x/wbuild/wbuild.cfg"
else
  MAKE_WBUILD=
  WBUILD="sh \$(PLTSRCDIR)/wxxt/src/XWidgets/dummy.wbuild"
fi

if test "$CC_FOR_BUILD" = "" ; then 
  CC_FOR_BUILD="$CC"
fi

############## platform tests ################

# for flags we don't want to use in config tests:
EXTRALIBS=

# for gmp assembly:
EXTRA_GMP_OBJ=

# For MzScheme targets:
OSX=".osx"
NOT_OSX=""

case $OS in
  SunOS)
    case `$UNAME -r` in
      5.*)
       STATICLINK="-Wl,-Bstatic" # after -ldl
       LIBS="$LIBS -lsocket -lnsl -lintl"
       need_gcc_static_libgcc="yes"
       check_gcc_dash_e="yes"
       ;;
      *)
       ;;
    esac
    ;;
  AIX)
    EXTRALIBS="-Wl,-brtl,-bE:\$(srcdir)/../mzscheme/include/mzscheme.exp"
    ;;
  FreeBSD)
    LIBS="$LIBS -rdynamic"
    ;;
  OpenBSD)
    LIBS="$LIBS -rdynamic"
    ;;
  IRIX)
    STATICLINK="/usr/lib/libC.a /usr/lib/libmalloc.a"
    if test "$X_PRE_LIBS" = " -lSM -lICE" ; then
      # For some reason, works best to drop these
      X_PRE_LIBS=""
    fi
    ;;
  Linux)
    LIBS="$LIBS -rdynamic"
    # PPC: X11 librares are not found
    case `$UNAME -m` in
      ppc)
       if test -d /usr/X11R6/lib ; then
         X_LIBS="$X_LIBS -L/usr/X11R6/lib"
       fi
       ;;
      alpha)
       EXTRA_GMP_OBJ="gmp_alpha_gcc.o"
       ;;
      *)
       ;;
    esac
    ;;
  OSF1)
    if test "$CC" = "cc" ; then 
      COMPFLAGS="$COMPFLAGS -ieee_with_inexact -assume noaligned_objects"
    elif test "$CC" = "gcc" ; then 
      EXTRA_GMP_OBJ="gmp_alpha_gcc.o"
    fi
    ;;
  HP-UX)
    if test "$CC" = "gcc"; then
      COMPFLAGS="$COMPFLAGS -fPIC"
    else
      CFLAGS="$CFLAGS -Wp,-H,64000"
      PREFLAGS="$PREFLAGS -D_HPUX_SOURCE"
      COMPFLAGS="$COMPFLAGS -Aa +Z"
      CGCOPTIONS="$CGCOPTIONS +ESdbgasm"
      MZOPTIONS="$MZOPTIONS +e"
    fi
    LIBS="$LIBS -Wl,-E"
    # For Xaw and Xmu:
    X_CFLAGS="-I/usr/contrib/X11R6/include/ $X_CFLAGS"
    X_LIBS="-L/usr/contrib/X11R6/lib/ $X_LIBS"
    ;;
  SCO) # How do you really recognize SCO?
    LIBS="$LIBS -lc -lsocket"
    ;;
  CYGWIN*)
    MZBINTARGET=cygwin-bin
    MZINSTALLTARGET=cygwin-install
    EXTRALIBS="$LIBS gmzwin.exp"
    MZOPTIONS="$MZOPTIONS -DGC_DLL"
    CGCOPTIONS="$CGCOPTIONS -DGC_DLL -DGC_BUILD -DUSE_MSVC_MD_LIBRARY -DMD_LIB_MAIN"
    ;;
  BeOS)
    if test "${enable_sgcdebug}" = "yes" ; then
      dummyvar=""
    else
      enable_sgc=yes
    fi
    ;;
  Darwin)

    PREFLAGS="$PREFLAGS -DOS_X"

    # zlib comes with the OS
    ZLIB_A=""
    ZLIB_INC=""

[   gcc_vers_three=`${CC} -v 2>&1 | grep "version [3-9][.]"` ]
    if test "$gcc_vers_three" = "" ; then
      # gcc 2.95.2
      if test "${enable_quartz}" = "yes" ; then
        MROPTIONS="$MROPTIONS -cpp-precomp"
      fi
    else
      # gcc 3.x
      CXXFLAGS="$CXXFLAGS -fno-rtti"
      if test "${enable_quartz}" = "yes" ; then
[       gcc_vers_three_one=`${CC} -v 2>&1 | grep "version 3[.]1"` ]
        if test "$gcc_vers_three_one" = "" ; then
          # gcc 3.3 and up
          GCC_VERSION_THREE_THREE="yes"
        else
          # gcc 3.1
          WXPRECOMP='$(WXPRECOMPDIR)/precomp.o'
          USE_WXPRECOMP='--load-pch $(WXPRECOMPDIR)'
        fi
      else
        MROPTIONS="$MROPTIONS -no-cpp-precomp"
      fi
    fi

    if test "${enable_quartz}" = "yes" ; then
      WXVARIANT="wx_mac"
      MROPTIONS="$MROPTIONS -fpascal-strings"
      INCLUDEDEP="-include"
      OPTIONS="$OPTIONS -fno-common"
      OSX=""
      NOT_OSX=".other"
      MZINSTALLTARGET=osx-install
      enable_xrender=no
      enable_cairo=no
      enable_shared=no
      enable_gl=no
      enable_pthread=no
      if test "${enable_libfw}" = "yes" ; then
        FRAMEWORK_INSTALL_DIR=/Library/Frameworks
      else
        FRAMEWORK_INSTALL_DIR=~/Library/Frameworks
      fi
    else
      PREFLAGS="$PREFLAGS -DXONX "
      X_EXTRA_LIBS="$X_EXTRA_LIBS -lz"
    fi
    ;;
  *)
    ;;
esac

MACH=`$UNAME -p`
case "$MACH" in
  alpha | sparc | powerpc)
    if test "$CC" = "gcc" ; then
      if test "$as_was_set" = "no" ; then
       AS="gcc -c -x assembler-with-cpp"
      fi
    fi
    ;; 
  *)
    ;;
esac

############## SGC ################

if test "${enable_sgc}" = "yes" ; then
  GCDIR=sgc
  OPTIONS="$OPTIONS -DUSE_SENORA_GC"
fi

if test "${enable_sgcdebug}" = "yes" ; then
  GCDIR=sgc
  OPTIONS="$OPTIONS -DSGC_STD_DEBUGGING=1"
fi

############## C flags ################

AC_LANG_C

[ msg="for inline keyword" ]
AC_MSG_CHECKING($msg)
AC_TRY_RUN(
   static inline int foo() { return 0; }
   int main() {
      return foo();
   }, inline=yes, inline=no, inline=no)
if test "$inline" = "no" ; then
  MZOPTIONS="$MZOPTIONS -DNO_INLINE_KEYWORD"
fi
AC_MSG_RESULT($inline) 

iconv_lib_flag=""
if test "${enable_iconv}" = "yes" ; then
  AC_CHECK_HEADER(iconv.h, enable_iconv=yes, enable_iconv=no)
  if test "${enable_iconv}" = "yes" ; then  
    # Does it all work, now?
    AC_TRY_RUN(
[   #include <iconv.h>]
[   #include <langinfo.h>]
    int main() {
[     iconv_open("UTF-8", "UTF-8");]
      return 0;
    }, enable_iconv=yes, enable_iconv=no, enable_iconv=yes)
    if test "${enable_iconv}" = "no" ; then  
      # Try adding -liconv ?
      #  We did not use AC_CHECK_LIB because iconv is sometimes macro-renamed
      ORIG_LIBS="$LIBS"
      LIBS="$LIBS -liconv"
      AC_TRY_RUN(
[     #include <iconv.h>]
[     #include <langinfo.h>]
      int main() {
[     iconv_open("UTF-8", "UTF-8");]
      return 0;
      }, enable_iconv=yes, enable_iconv=no, enable_iconv=yes)
      if test "${enable_iconv}" = "no" ; then
        LIBS="$ORIG_LIBS"
      else
        iconv_lib_flag=" -liconv"      
      fi
    fi
  fi
fi
[ msg="iconv is usable" ]
AC_MSG_CHECKING($msg)
iconv_usage_result="$enable_iconv$iconv_lib_flag"
AC_MSG_RESULT($iconv_usage_result)
if test "${enable_iconv}" = "no" ; then
  MZOPTIONS="$MZOPTIONS -DMZ_NO_ICONV"  
fi

[ msg="for mbsrtowcs" ]
AC_MSG_CHECKING($msg)
AC_TRY_RUN(
[  #include <wchar.h> ]
   int main() {
     mbstate_t state;
[    char *src = "X";]
[    mbsrtowcs(0, &src, 0, &state);]
     return 0;
   }, mbsrtowcs=yes, mbsrtowcs=no, mbsrtowcs=no)
if test "$mbsrtowcs" = "no" ; then
  MZOPTIONS="$MZOPTIONS -DNO_MBTOWC_FUNCTIONS"
fi
AC_MSG_RESULT($mbsrtowcs)

if test "${enable_backtrace}" = "yes" ; then
  enable_compact=yes
  GC2OPTIONS="$GC2OPTIONS -DCOMPACT_BACKTRACE_GC"
fi

if test "${enable_compact}" = "yes" ; then
  GC2OPTIONS="$GC2OPTIONS -DUSE_COMPACT_3M_GC"
  enable_account=no
  enable_memtrace=no
fi

if test "${enable_account}" = "yes" ; then
  GC2OPTIONS="$GC2OPTIONS -DNEWGC_BTC_ACCOUNT"
fi

if test "${enable_memtrace}" = "yes" ; then
  GC2OPTIONS="$GC2OPTIONS -DNEWGC_MEMORY_TRACE"
fi

############## Solaris grunge ################

if test "$check_gcc_dash_e" = "yes" ; then

  orig_ldflags="$LDFLAGS"
  LDFLAGS="$LDFLAGS -Wl,-E"

  # Can use -Wl,-E for linking because we're using GNU linker?
[ msg="whether linker accepts -Wl,-E"   ]
  AC_MSG_CHECKING($msg)
  AC_TRY_RUN(
   int main() {
      return 0;
   }, linker_dash_e=yes, linker_dash_e=no, linker_dash_e=no)
  if test "$linker_dash_e" = "no" ; then
    LDFLAGS="${orig_ldflags}"
  fi
  AC_MSG_RESULT($linker_dash_e)

fi

############## C++ grunge ################

if test "${enable_mred}" = "yes" ; then
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"
  AC_LANG_CPLUSPLUS 

  AC_MSG_CHECKING(whether C++ compiler works)
  AC_TRY_RUN(int main() { return 0; }, cplusplusworks=yes, cplusplusworks=no, cplusplusworks=yes)
  AC_MSG_RESULT($cplusplusworks) 
  if test "$cplusplusworks" = "no" ; then
    MROPTIONS="$MROPTIONS -DOPERATOR_NEW_ARRAY"
    echo "***************************************************"
    echo configure: C++ compiler/linker ${CXX} does not work
    echo "***************************************************"
    echo configure aborted
    exit 1
  fi 

[ msg="whether new and new[] are different"   ]
  AC_MSG_CHECKING($msg)
  AC_TRY_RUN(
[  #include <stddef.h> ]
   inline void *operator new(size_t size) { return (void *)0x1; }
[  inline void *operator new[](size_t size) { return (void *)0x2; }  ]
   class C { int x; };
   int main() {
[    return (new C) == (new C[10]);       ]
   }, different=yes, different=no, different=yes)
  if test "$different" = "yes" ; then
    MROPTIONS="$MROPTIONS -DOPERATOR_NEW_ARRAY"
  fi
  AC_MSG_RESULT($different) 

  # If we're using gcc and including X11/Intrinsic doesn't work
  #  then try adding -fpermissive
  if test "$CC" = "gcc" ; then
    AC_MSG_CHECKING(whether need to use -fpermissive)
    AC_TRY_COMPILE([#]include "X11/Intrinsic.h", , addperm=no, addperm=yes)
    if test "$addperm" = "yes" ; then
      CXXFLAGS="$CXXFLAGS -fpermissive"
    fi
    AC_MSG_RESULT($addperm) 
  fi

  if test "$enable_quartz" = "no" ; then

  # Can we make auto-repeat detectable?
[ msg="whether X11/XKBlib.h is available"   ]
  AC_MSG_CHECKING($msg)
  AC_TRY_RUN(
[  #include <X11/Intrinsic.h>]
[  #include <X11/XKBlib.h>]
   int main() {
      return 0;
   }, xkbavail=yes, xkbavail=no, xkbavail=no)
  if test "$xkbavail" = "no" ; then
    MROPTIONS="$MROPTIONS -DNO_XKB_LIB_PRESENT"
  fi
  AC_MSG_RESULT($xkbavail)

 fi # non-quartz

fi

############## drop optimization flags ################

if test "${enable_noopt}" = "yes" ; then
  AWKPRG='BEGIN { FS = "(^| )-O(0|1|2|3|4|5|6|7|8|9|())( |$)" } /.*/ { for (i = 1; i < NF; i++) printf "%s ", $i; print $NF }'
  CFLAGS=`echo "$CFLAGS" | awk "$AWKPRG"`
  CXXFLAGS=`echo "$CXXFLAGS" | awk "$AWKPRG"`
fi

################ X OpenGL ##################

if test "${enable_gl}" = "" ; then
  enable_gl=yes
fi

if test "${enable_pthread}" = "" ; then
  enable_pthread=no
fi

if test "${enable_gl}" = "yes" ; then  
  # Check for header
  AC_CHECK_HEADER(GL/glx.h, GLX_HEADER=yes, GLX_HEADER=no)
  if test "${GLX_HEADER}" = "no" ; then  
    echo "could not find GL/glx.h header; disabling GL support"
    enable_gl=no
  fi
fi

if test "${enable_gl}" = "yes" ; then 
  AC_CHECK_LIB(GL, glXChooseVisual, GLX_LINK=yes, GLX_LINK=no, $X_LIBS -lX11 -lXext)
  if test "${GL_LINK}" = "no" ; then
    AC_CHECK_LIB(GL, glXChooseVisual, GLX_LINK=pthread, GLX_LINK=no, $X_LIBS -lX11 -lXext -pthread)
  fi
  if test "${GLX_LINK}" = "no" ; then
    echo "could not link to the GLX library; disabling GL support"
    enable_gl=no
  fi
  if test "${GLX_LINK}" = "pthread" ; then
    if test "${enable_pthread}" = "no" ; then
      echo "could not link to GLX without pthreads; use --enable-pthread to enable GL"
      enable_gl=no
    fi
  fi 
fi

if test "${enable_gl}" = "yes" ; then
  X_EXTRA_LIBS="$X_EXTRA_LIBS -lGL"
  MROPTIONS="$MROPTIONS -DUSE_GL"
fi

############### pthread ###################

if test "${enable_pthread}" = "yes" ; then
  PREFLAGS="$PREFLAGS -D_THREAD_SAFE"
  X_EXTRA_LIBS="$X_EXTRA_LIBS -pthread"
  MZOPTIONS="$MZOPTIONS -DASSUME_FIXED_STACK_SIZE -DFIXED_STACK_SIZE=1048576 -DDONT_ITIMER"
  GC2OPTIONS="$GC2OPTIONS -DNO_GC_SIGNALS"
fi


################ Xrender ##################

if test "${enable_xrender}" = "" ; then  
  enable_xrender=yes
fi

if test "${enable_xrender}" = "yes" ; then  
  AC_CHECK_HEADER(X11/extensions/Xrender.h, enable_xrender=yes, enable_xrender=no)
  if test "${enable_xrender}" = "yes" ; then  
    AC_CHECK_LIB(Xrender, XRenderQueryExtension, enable_xrender="yes", enable_xrender="no", $X_LIBS -lX11 -lXext -lXrender)
    if test "${enable_xrender}" = "yes" ; then  
      MROPTIONS="$MROPTIONS -DWX_USE_XRENDER"
      X_EXTRA_LIBS="$X_EXTRA_LIBS -lXrender"
    else
      echo "no Xrender libs; Xrender support disabled"
    fi
  else
    echo "no Xrender headers; Xrender support disabled"
  fi
fi

################ Xft ##################

if test "${enable_xft}" = "" ; then  
  if test "${enable_xrender}" = "yes" ; then  
    enable_xft=yes
  else
    enable_xft=no
  fi
fi

if test "${enable_xft}" = "yes" ; then  
  xft_config_ok=`xft-config --cflags  2> /dev/null`
  if test "$xft_config_ok" = "" ; then
    AC_CHECK_HEADER(X11/Xft/Xft.h, enable_xft=yes, enable_xft=no)
    if test "${enable_xft}" = "yes" ; then  
      AC_CHECK_LIB(Xft, XftTextExtents8, enable_xft="yes", enable_xft="no", $X_LIBS -lX11 -lXext -Xrender -lXft)
      if test "${enable_xft}" = "yes" ; then  
        MROPTIONS="$MROPTIONS -DWX_USE_XFT"
        X_EXTRA_LIBS="$X_EXTRA_LIBS -lXft"
      else
        echo "no Xft libs; Xft support disabled"
      fi
    else
      echo "no Xft headers; Xft support disabled"
    fi
  else
    echo "Using xft-config..."
    xft_cflags=`xft-config --cflags`
    xft_libs=`xft-config --libs`
    echo "Xft flags: $xft_cflags"
    echo "Xft libs: $xft_libs"
    MROPTIONS="$MROPTIONS -DWX_USE_XFT"
    X_CFLAGS="$X_CFLAGS $xft_cflags"
    X_EXTRA_LIBS="$X_EXTRA_LIBS $xft_libs"
  fi
fi

################ Cairo ##################

if test "${enable_cairo}" = "" ; then  
  enable_cairo=yes
fi

if test "${enable_cairo}" = "yes" ; then  
  cairo_config_ok=`pkg-config --cflags cairo 2> /dev/null`
  if test "$cairo_config_ok" = "" ; then
    echo "no information from pkg-config; Cairo support disabled"
  else
    echo "Using pkg-config for Cairo..."
    cairo_cflags=`pkg-config --cflags cairo`
    cairo_libs=`pkg-config --libs cairo`
    echo "Cairo flags: $cairo_cflags"
    echo "Cairo libs: $cairo_libs"
    AC_CHECK_LIB(cairo, cairo_create, enable_cairo="yes", enable_xrender="no", $X_LIBS $X_EXTRA_LIBS $cairo_libs)
    if test "${enable_cairo}" = "yes" ; then  
      MROPTIONS="$MROPTIONS -DWX_USE_CAIRO"
      X_CFLAGS="$X_CFLAGS $cairo_cflags"
      X_EXTRA_LIBS="$X_EXTRA_LIBS $cairo_libs"

      CPPFLAGS="$CPPFLAGS $cairo_cflags"
      AC_CHECK_HEADER(cairo-xlib.h, havecairoxlib=yes, havecairoxlib=no)
      if test "${havecairoxlib}" = "no" ; then  
        MROPTIONS="$MROPTIONS -DWX_CAIRO_NO_XLIBH"
      fi
    else
      echo "no Cairo libs; Cairo support disabled"
    fi
  fi
fi

################ libpng ##################

if test "${enable_libpng}" = "" ; then  
  enable_libpng=yes
fi

if test "${enable_libpng}" = "yes" ; then  
  AC_CHECK_HEADER(png.h, enable_libpng=yes, enable_libpng=no)
  if test "${enable_libpng}" = "yes" ; then  
    AC_CHECK_LIB(png, png_read_info, enable_libpng="yes", enable_libpng="no", -lpng -lz)
    if test "${enable_libpng}" = "yes" ; then  
      PNG_A=""
      PNG_INC=""
      ZLIB_A=""
      ZLIB_INC=""
      X_EXTRA_LIBS="$X_EXTRA_LIBS -lpng -lz"
      MROPTIONS="$MROPTIONS -DWX_USE_LIBPNG"
    else
      echo "no libpng installed; building static version"
    fi
  else
    echo "no libpng headers installed; building from PLT copy"
  fi
fi

################ libjpeg ##################

if test "${enable_libjpeg}" = "" ; then  
  enable_libjpeg=yes
fi

if test "${enable_libjpeg}" = "yes" ; then  
  AC_CHECK_HEADER(jpeglib.h, enable_libjpeg=yes, enable_libjpeg=no)
  if test "${enable_libjpeg}" = "yes" ; then  
    AC_CHECK_LIB(jpeg, jpeg_start_decompress, enable_libjpeg="yes", enable_libjpeg="no", -ljpeg)
    if test "${enable_libjpeg}" = "yes" ; then  
      JPEG_A=""
      JPEG_INC=""
      X_EXTRA_LIBS="$X_EXTRA_LIBS -ljpeg"
      MROPTIONS="$MROPTIONS -DWX_USE_LIBJPEG"
    else
      echo "no libjpeg installed; building static version"
    fi
  else
    echo "no libjpeg headers installed; building from PLT copy"
  fi
fi

############## libtool ################

if test "${enable_shared}" = "yes" ; then
  echo "Configuring libtool"

  if test ! -d "lt" ; then
    mkdir "lt"
  fi
  abssrcdir=`(cd ${srcdir}; pwd)`

  # Try to convert prefix to an absolute path:
  if test -d "${prefix}" ; then
    absprefix=`(cd ${prefix}; pwd)`
  else
    absprefix="${prefix}"
  fi

  (cd lt; sh ${abssrcdir}/lt/configure --enable-shared --disable-static)
  LTDIR=`pwd`/lt

  if test "${need_gcc_static_libgcc}" = "yes" ; then
    need_gcc_static_libgcc=""
    if test "$CC" = "gcc" ; then 
      gcc_vers_three=`${CC} -v 2>&1 | grep "version 3[.]"`
      if test "$gcc_vers_three" = "" ; then
        need_gcc_static_libgcc=""
      else
        need_gcc_static_libgcc=" -XCClinker -static-libgcc"
      fi
    fi
  fi

  if test "$INCLUDEDEP" = "-include" ; then
    plt_lib_version="\$(FWVERSION)"
  else
    plt_lib_version=`grep " MZSCHEME_VERSION " ${srcdir}/mzscheme/src/schvers.h | cut -d '"' -f 2`
  fi

  AR="${LTDIR}/libtool --mode=link $CC${need_gcc_static_libgcc} -release ${plt_lib_version} -rpath ${absprefix}/lib -o"
  STATIC_AR="${LTDIR}/libtool --mode=link $CC -o"
  ARFLAGS=""
  RANLIB=":"
  MZLINKER="${LTDIR}/libtool --mode=link $CC${need_gcc_static_libgcc} -rpath ${absprefix}/lib"
  MREDLINKER="${LTDIR}/libtool --mode=link $CXX${need_gcc_static_libgcc} -rpath ${absprefix}/lib"
  PLAIN_CC="$CC"
  CC="${LTDIR}/libtool --mode=compile $CC"
  CXX="${LTDIR}/libtool --mode=compile $CXX"
  AS="${LTDIR}/libtool --mode=compile $AS"
  LIBSFX=la
  WXLIBS=WXLIBSDYN
  ICP="${LTDIR}/libtool --mode=install cp"
  MRLIBINSTALL="install-lib"
  LIBFINISH="${LTDIR}/libtool --mode=finish"
  LTO="lo"
  LTA="la"
  FOREIGN_CONVENIENCE="_convenience"
  FOREIGN_OBJSLIB="\$(FOREIGN_LIB)"
else
  LIBSFX=a
  MREDLINKER="$CXX"
  WXLIBS=WXLIBSNORM
  ICP=cp
  MRLIBINSTALL="install-no-lib"
  LIBFINISH=echo
  LTO="o"
  LTA="a"
  MZLINKER="$CC"
  STATIC_AR="$AR"
  PLAIN_CC="$CC"
  FOREIGN_CONVENIENCE=""
  FOREIGN_OBJSLIB="\$(FOREIGN_OBJS)"
fi

############## final output ################

LIBS="$LIBS $EXTRALIBS"

AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(CXX)
AC_SUBST(CXXFLAGS)
AC_SUBST(PREFLAGS)
AC_SUBST(COMPFLAGS)
AC_SUBST(PROFFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(CPP)
AC_SUBST(CXXCPP)
AC_SUBST(SED)
AC_SUBST(PERL)
AC_SUBST(AS)
AC_SUBST(RANLIB)
AC_SUBST(AR)
AC_SUBST(STATIC_AR)
AC_SUBST(ARFLAGS)
AC_SUBST(WBUILD)
AC_SUBST(CC_FOR_BUILD)
AC_SUBST(REZ)
AC_SUBST(MZLINKER)
AC_SUBST(PLAIN_CC)

AC_SUBST(x_includes)
AC_SUBST(x_libraries)
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)
AC_SUBST(X_PRE_LIBS)
AC_SUBST(X_EXTRA_LIBS)

AC_SUBST(OPTIONS)
AC_SUBST(MZOPTIONS)
AC_SUBST(CGCOPTIONS)
AC_SUBST(GC2OPTIONS)
AC_SUBST(MROPTIONS)
AC_SUBST(GCDIR)
AC_SUBST(MZBINTARGET)
AC_SUBST(MZINSTALLTARGET)
AC_SUBST(EXTRA_GMP_OBJ)
AC_SUBST(OSX)
AC_SUBST(NOT_OSX)
AC_SUBST(FRAMEWORK_INSTALL_DIR)

AC_SUBST(MREDLINKER)
AC_SUBST(LIBSFX)
AC_SUBST(WXLIBS)
AC_SUBST(WXVARIANT)
AC_SUBST(ICP)
AC_SUBST(MRLIBINSTALL)
AC_SUBST(LIBFINISH)

AC_SUBST(MAKE_MRED)
AC_SUBST(MAKE_MRED3M)
AC_SUBST(MAKE_MREDINSTALL)
AC_SUBST(MAKE_MREDINSTALL3M)
AC_SUBST(MAKE_WBUILD)
AC_SUBST(MAKE_COPYTREE)
AC_SUBST(MAKE_FINISH)

AC_SUBST(WXPRECOMP)
AC_SUBST(USE_WXPRECOMP)
AC_SUBST(INCLUDEDEP)
AC_SUBST(WX_MMD_FLAG)

AC_SUBST(JPEG_A)
AC_SUBST(JPEG_INC)
AC_SUBST(ZLIB_A)
AC_SUBST(ZLIB_INC)
AC_SUBST(PNG_A)

AC_SUBST(OSKHOME)
AC_SUBST(EXTRA_OSK_LIBS)

AC_SUBST(FOREIGN_OBJSLIB_IF_USED)
AC_SUBST(FOREIGN_LIB_IF_USED)
AC_SUBST(FOREIGN_OBJSLIB)
AC_SUBST(FOREIGN_CONVENIENCE)
AC_SUBST(FOREIGNTARGET)

AC_SUBST(LTO)
AC_SUBST(LTA)

mk_needed_dir()
{
  if test ! -d "$1" ; then
    mkdir "$1"
  fi
}

makefiles="Makefile
           mzscheme/Makefile
             mzscheme/src/Makefile mzscheme/dynsrc/Makefile
             mzscheme/gc/Makefile mzscheme/sgc/Makefile
             mzscheme/gc2/Makefile"

# Make sure the --enable-shared default is propagated:
if test "${enable_shared}" = "yes" ; then
  ac_configure_args="$ac_configure_args --enable-shared"
fi

FOREIGNTARGET=
FOREIGN_OBJSLIB_IF_USED=
FOREIGN_LIB_IF_USED=
if test -d "${srcdir}/foreign" && test "${enable_foreign}" = "yes" ; then
  AC_CONFIG_AUX_DIR( foreign/gcc )
  AC_CONFIG_SUBDIRS( foreign/gcc/libffi )
  makefiles="$makefiles foreign/Makefile"
  FOREIGNTARGET="foreign-stuff"
  FOREIGN_OBJSLIB_IF_USED="\$(FOREIGN_OBJSLIB)"
  FOREIGN_LIB_IF_USED="\$(FOREIGN_OBJ) \$(FOREIGN_LIB)"
  # --disable-multilib is needed to avoid problems when running libffi's
  # configure script.  This thing affects all usages of AC_CONFIG_SUBDIRS, but
  # other scripts do not use it anyway.
  ac_configure_args="$ac_configure_args --disable-multilib"
else
  FOREIGNTARGET="foreign-nothing"
  MZOPTIONS="$MZOPTIONS -DDONT_USE_FOREIGN"
fi

if test "${enable_mred}" = "yes" ; then
  if test "${enable_quartz}" = "yes" ; then
    mk_needed_dir wxmac
    mk_needed_dir wxmac/src
  else
    mk_needed_dir wxxt
    mk_needed_dir wxxt/utils
    mk_needed_dir wxxt/utils/image
    mk_needed_dir wxxt/contrib
    mk_needed_dir wxxt/contrib/xpm
  fi

  makefiles="$makefiles
             mred/Makefile
               mred/wxs/Makefile mred/wxme/Makefile
               mred/gc2/Makefile
               wxcommon/libpng/Makefile wxcommon/zlib/Makefile"

  if test "${enable_quartz}" = "yes" ; then
    makefiles="$makefiles
               wxmac/src/Makefile"
  else
    makefiles="$makefiles
               wxxt/src/Makefile wxxt/src/x/Makefile
                 wxxt/utils/image/src/Makefile
                 wxxt/contrib/xpm/lib/Makefile"
  fi

  if test "${enable_libjpeg}" = "no" ; then
    AC_CONFIG_AUX_DIR( wxcommon/jpeg )
    AC_CONFIG_SUBDIRS( wxcommon/jpeg )
  fi
fi

if test "${enable_wbuild}" = "yes" ; then
  AC_CONFIG_AUX_DIR( wxxt/src/x/wbuild )
  AC_CONFIG_SUBDIRS( wxxt/src/x/wbuild )
fi

AC_OUTPUT($makefiles)

