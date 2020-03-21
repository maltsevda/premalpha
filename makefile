# variables

CC = cl
CFLAGS = /nologo /W4 /utf-8 /I "./glut" /I "./libpng"
DEFLAGS = /D "WIN32"
LD = link
LDFLAGS = /nologo /LIBPATH:"./glut" /LIBPATH:"./libpng"
RC = rc
RCFLAGS = /nologo /r
RM = del

# static libraries for linker

SRCS = main.c png.c
LIBS = User32.lib Gdi32.lib

OBJS = $(SRCS:.cpp=.obj)
OBJS = $(OBJS:.c=.obj)
OBJS = $(OBJS:.rc=.res)

# rules

.PHONY: all

all: main.exe

main.exe: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS)
	$(RM) *.obj
	move /Y $@ release\$@

.rc.res:
	$(RC) $(RCFLAGS) $(DEFLAGS) /Fo$@ $<

.c*.obj:
	$(CC) /c $(CFLAGS) $(DEFLAGS) /Fo$@ $<
