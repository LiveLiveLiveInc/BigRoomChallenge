DEBUG  ?= 1

RM=rm -f

LDLIBS=-lc++ -lopencv_core -lopencv_highgui -lopencv_imgcodecs  -lopencv_imgproc

CXX=clang++
CPPFLAGS=-std=c++17 -stdlib=libc++ -I/usr/local/opt/opencv/include/opencv4
LDFLAGS=-L/usr/local/opt/opencv/lib

ifeq ($(DEBUG), 1)
	CPPFLAGS +=-g -Wall -pedantic
	LDFLAGS +=-g
else
	CPPFLAGS += -O3 -DNDEBUG
endif

all: test

test: $(OBJS) ./main.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o test $^ $(LDLIBS)

clean:
	$(RM) test
	$(RM) file.uyvy
	$(RM) -rf *.dSYM
	