SRCS ?=
PROG ?=

CC=gcc
CXX=g++
RM=rm -f
INCLUDES = -I/home/user/libzmq/include -I../../ -I../
CXXFLAGS := -std=c++1y -g -Wall ${INCLUDES}
LDFLAGS = -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu 
LIBS= -lzmq -lboost_serialization -lboost_system -lboost_filesystem -lboost_thread 

OBJS= $(subst .cpp,.o,$(SRCS))

all: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(PROG) $(OBJS) $(LIBS) $(LDFLAGS)

.PHONY:
clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(PROG)