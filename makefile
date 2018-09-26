###############################################################################
# Makefile for PVRX2PNG by [big_fury]SiZiOUS
# http://sbibuilder.shorturl.com/
# Based on squishpng sample
#
# This project is using libpng, zlib and libsquish.
# Static libraries are automatically built if needed.
#
# This Makefile is for mingw32-make. 
# I don't know if it works for other make tool...
#
# Oh yeah, this makefile is NOT standard, and yes, I DON'T GIVE A F*CK!!!
# It works, who cares ???
# In fact I was unable to run any makefile for theses libraries... so I made
# them again!
###############################################################################

# Project Name
PRJNAME 	= pvrx2png

# Debug or not ?
DEBUG		= False

# Project files
OBJS 		= $(OBJSDIR)main.o $(OBJSDIR)rsrc.o

# Directories
BINDIR 		= bin/
OBJSDIR 	= obj/
LIBDIR		= lib/

# Proggies
AR 			= ar
CC 			= gcc
CXX 		= g++
RM 			= rm
STRIP 		= strip
UPX 		= upx
WINDRES 	= windres

# Flags
CXXFLAGS 	= -O3 -Wall -I"./deps/zlib/" -I"./deps/libpng/" -I"./deps/squish"
LDFLAGS 	=  -L"./lib" -lstdc++ -lsquish -lpng -lz
UPXFLAGS 	= -9

###############################################################################
# MAKE TARGET PVRX2PNG
###############################################################################

# Target binary
TARGET = $(BINDIR)$(PRJNAME).exe

# Debug test...
ifeq ($(DEBUG),True)
CXXFLAGS += -DDEBUG
endif

# Go !	
all : libsquish-make libpng-make $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) $(CXXFLAGS)
	$(STRIP) $(TARGET)
	$(UPX) $(UPXFLAGS) $(TARGET)
	
# Compile sources
$(OBJSDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
# Include win32 resources
$(OBJSDIR)rsrc.o: rsrc.rc
	$(WINDRES) -i rsrc.rc -o $(OBJSDIR)rsrc.o
	
# Clean	
clean:
	$(RM) $(OBJS) $(LIBSQUISH_OBJS) $(LIBPNG_OBJS) $(ZLIB_OBJS)
	
clean-all:
	$(RM) $(OBJS) $(LIBSQUISH_OBJS) $(LIBPNG_OBJS) $(ZLIB_OBJS) $(TARGET) \
			$(LIBSQUISH_STATICLIB) $(ZLIB_STATICLIB) $(LIBPNG_STATICLIB)
	
###############################################################################	
# MAKE LIBSQUISH STATIC LIBRARY
###############################################################################

LIBSQUISH_ARFLAGS 	= 	rcs
LIBSQUISH_SRCDIR	= 	./deps/squish/
LIBSQUISH_STATICLIB	= 	$(LIBDIR)libsquish.a
LIBSQUISH_CXXFLAGS	= 	-O2 -I"$(LIBSQUISH_SRCDIR)"
LIBSQUISH_OBJS 		= 	$(OBJSDIR)alpha.o $(OBJSDIR)clusterfit.o $(OBJSDIR)colourblock.o \
						$(OBJSDIR)colourfit.o $(OBJSDIR)colourset.o $(OBJSDIR)maths.o \
						$(OBJSDIR)rangefit.o $(OBJSDIR)singlecolourfit.o \
						$(OBJSDIR)squish.o

libsquish-make: $(LIBSQUISH_OBJS)
	$(AR) $(LIBSQUISH_ARFLAGS) $(LIBSQUISH_STATICLIB) $(LIBSQUISH_OBJS)

$(OBJSDIR)%.o: $(LIBSQUISH_SRCDIR)%.cpp
	$(CXX) $(LIBSQUISH_CXXFLAGS) -c $< -o $@ 
	
libsquish-clean:
	$(RM) $(LIBSQUISH_OBJS)
	
###############################################################################
# MAKE LIBPNG STATIC LIBRARY
###############################################################################

LIBPNG_ARFLAGS		=	rcs
LIBPNG_SRCDIR 		= 	./deps/libpng/
LIBPNG_STATICLIB 	= 	$(LIBDIR)libpng.a
LIBPNG_CFLAGS		= 	-O3 -Wall -L"$(LIBDIR)" -I"$(ZLIB_SRCDIR)"
LIBPNG_OBJS			=	$(OBJSDIR)png.o $(OBJSDIR)pngset.o $(OBJSDIR)pngget.o \
						$(OBJSDIR)pngrutil.o $(OBJSDIR)pngtrans.o $(OBJSDIR)pngwutil.o \
						$(OBJSDIR)pngread.o $(OBJSDIR)pngrio.o $(OBJSDIR)pngwio.o \
						$(OBJSDIR)pngwrite.o $(OBJSDIR)pngrtran.o \
						$(OBJSDIR)pngwtran.o $(OBJSDIR)pngmem.o $(OBJSDIR)pngerror.o \
						$(OBJSDIR)pngpread.o


libpng-make: zlib-make $(LIBPNG_OBJS)
	$(AR) $(LIBPNG_ARFLAGS) $(LIBPNG_STATICLIB) $(LIBPNG_OBJS)

$(OBJSDIR)%.o: $(LIBPNG_SRCDIR)%.c
	$(CC) $(LIBPNG_CFLAGS) -c $< -o $@ 
	
libpng-clean:
	$(RM) $(LIBPNG_OBJS)
	
###############################################################################
# MAKE ZLIB STATIC LIBRARY
###############################################################################

ZLIB_ARFLAGS	=	rcs
ZLIB_SRCDIR		= 	./deps/zlib/
ZLIB_STATICLIB 	= 	$(LIBDIR)libz.a
ZLIB_CFLAGS		=	-O3 -Wall
ZLIB_OBJS 		= 	$(OBJSDIR)adler32.o $(OBJSDIR)compress.o $(OBJSDIR)crc32.o \
					$(OBJSDIR)deflate.o $(OBJSDIR)gzio.o $(OBJSDIR)infback.o \
					$(OBJSDIR)inffast.o $(OBJSDIR)inflate.o $(OBJSDIR)inftrees.o \
					$(OBJSDIR)trees.o $(OBJSDIR)uncompr.o $(OBJSDIR)zutil.o
					
zlib-make: $(ZLIB_OBJS)
	$(AR) $(ZLIB_ARFLAGS) $(ZLIB_STATICLIB) $(ZLIB_OBJS)

$(OBJSDIR)%.o: $(ZLIB_SRCDIR)%.c
	$(CC) $(ZLIB_CFLAGS) -c $< -o $@

zlib-clean:
	$(RM) $(ZLIB_OBJS)