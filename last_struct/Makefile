# SPDX-License-Identifier: BSD-3-Clause
# Copyright 2015-2016, Intel Corporation

#
#

PROGS = pm_dyn_str

LIBS = -lpmemobj -lpmem -pthread

LINKER=$(CC)
ifeq ($(COMPILE_LANG), cpp)
LINKER=$(CXX)
endif


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


pm_dyn_str: pm_dyn_str.o

clean:
	rm -f $(PROGS)
	rm -f *.o
