PROGS = lista_enc_completa lista_enc_final lista_enc_cabeca

LIBS = -lpmemobj -lpmem -pthread

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


lista_enc_completa: lista_enc_completa.o
lista_enc_final: lista_enc_final.o
lista_enc_cabeca: lista_enc_cabeca.o

clean:
	rm -f $(PROGS)
	rm -f *.o
