SRCS ?=
PROG ?=
BINDIR ?=
OBJDIR ?=

CC=gcc
CXX=g++
RM=rm -f
INCLUDES = -I/home/user/libzmq/include -I../../ -I../ -I/usr/local/include
CXXFLAGS := -std=c++1y -g -Wall ${INCLUDES}
LDFLAGS = -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu 
LIBS= -lzmq -lboost_serialization -lboost_system -lboost_filesystem -lboost_thread -lpugixml

vpath %.cpp $(dir $(SRCS))

SRCNAMES = $(notdir $(SRCS))
OBJS = $(patsubst %.cpp, $(OBJDIR)/$(PROG)/%.o, $(SRCNAMES))

$(BINDIR)/$(PROG): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $@ $(LIBS) $(LDFLAGS)

$(OBJS): | $(OBJDIR)/$(PROG)

$(OBJDIR)/$(PROG):
	@mkdir -p $@

$(OBJDIR)/$(PROG)/%.o : %.cpp
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS) $(LDFLAGS)

.PHONY:
clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(BINDIR)/$(PROG)