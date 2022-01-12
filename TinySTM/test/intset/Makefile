ROOT ?= ../..

include $(ROOT)/Makefile.common

LDFLAGS += -lpmemobj

BINS = intset-hs intset-hs-pm intset-ll intset-ll-pm intset-ll-pm-DB intset-rb intset-sl

UNAME := $(shell uname)
ifeq ($(UNAME), SunOS)
# Solaris requires rt lib for nanosleep
LDFLAGS += -lrt
endif

.PHONY:	all clean

all:	$(BINS)

intset-hs.o:	intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_HASHSET -DNO_STM -c -o $@ $<

intset-hs-pm.o: intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_HASHSET -DNO_STM -DPERSISTENT -c -o $@ $<

intset-ll.o:	intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_LINKEDLIST -DNO_STM -c -o $@ $<
	
intset-ll-pm.o: intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_LINKEDLIST -DNO_STM -DPERSISTENT -c -o $@ $<
	
intset-ll-pm-DB.o: intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_LINKEDLIST -DNO_STM -DPERSISTENT -DDEBUG_PM -c -o $@ $<

intset-rb.o:	intset.c rbtree.c rbtree.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_RBTREE -DNO_STM -c -o $@ $<

intset-sl.o:	intset.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEFINES) -DUSE_SKIPLIST -DNO_STM -c -o $@ $<

# FIXME in case of ABI $(TMLIB) must be replaced to abi/...
$(BINS):	%:	%.o $(TMLIB)
	$(LD) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(BINS) *.o
