PROGS = stack

LIBS = -lpmemobj

LINKER=$(CC)
ifeq ($(COMPILE_LANG), cpp)
LINKER=$(CXX)
endif

CFLAGS= -g -Wall -Wextra

MAKEFILE_DEPS=Makefile

all: $(PROGS)

%.o: %.c $(MAKEFILE_DEPS)
	$(call check-cstyle, $<)
	$(CC) -c -o $@ $(CFLAGS) $(INCS) $<

%.o: %.cpp $(MAKEFILE_DEPS)
	$(call check-cstyle, $<)
	$(CXX) -c -o $@ $(CXXFLAGS) $(INCS) $<

$(PROGS):
	$(LINKER) -o $@ $^ $(LDFLAGS) $(LIBS)


stack: stack.o

clean:
	rm -f $(PROGS)
	rm -f *.o



