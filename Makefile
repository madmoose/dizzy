CXX = g++
CXXFLAGS:= -g -O1 -Wall -Wuninitialized -Wno-unknown-pragmas
CXXFLAGS+= -Wimplicit -Wundef -Wreorder -Wwrite-strings -Wnon-virtual-dtor -Wno-multichar

SRCS_dizzy = main.cpp \
             functions.cpp \
             ioutils.cpp \
             interrupts.cpp \
	     names.cpp \
             register_trace.cpp \
             segments.cpp \
             x86/x86_disasm.cpp

OBJS_dizzy = $(SRCS_dizzy:.cpp=.o)
DEPS_dizzy = $(SRCS_dizzy:.cpp=.d)

all: dizzy

dizzy: $(OBJS_dizzy)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS_dizzy)

.cpp.o:
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $*.o

clean:
	rm -rf dizzy *.o *.d x86/*.o x86/*.d *.dSYM a.out

-include $(DEPS)
