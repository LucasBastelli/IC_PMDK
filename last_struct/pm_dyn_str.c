#include <stdio.h>
#include <string.h>
#include <libpmemobj.h>                                                              

/*
 Create the pool for this app with:
 pmempool create obj --layout=strdynamic <pool-name>
*/

#define LAYOUT_NAME "strdynamic"
#define MAX_BUF_LEN 100

struct conjunto{
	char string[100];
	int numero;
};

POBJ_LAYOUT_BEGIN(string_dynamic);
POBJ_LAYOUT_ROOT(string_dynamic, struct my_root);
POBJ_LAYOUT_TOID(string_dynamic, struct conjunto);
POBJ_LAYOUT_END(string_dynamic);



struct my_root {
  TOID(struct conjunto) my_string;
};




static void string_print(const TOID(struct conjunto) str)
{
	const struct conjunto *aux= D_RO(str);
	printf("String: %s\n", aux->string);
	printf("Numero: %d\n", aux->numero);
}


  
int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("usage: %s file-name\n", argv[0]);
    return 1;
  }

  PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
  if (pop == NULL) {
    perror("pmemobj_open");
    return 1;
  }

  TOID(struct my_root) root = POBJ_ROOT(pop, struct my_root);
  if (!TOID_IS_NULL(D_RO(root)->my_string))
    {
	string_print(D_RO(root)->my_string);
    	POBJ_FREE(&D_RW(root)->my_string);
  }

  struct conjunto buf;
  printf(" \nString: ");
  if (scanf("%s", buf.string) == EOF) {
    fprintf(stderr, "EOF\n");
    return 1;
    }
  printf(" Numero: ");
  if (scanf("%d", &buf.numero) == EOF) {
    fprintf(stderr, "EOF\n");
    return 1;
    }
  
  

	printf("\nAdicionando elemento\n");
  TX_BEGIN(pop) {

    // alloc & copy
    TOID(struct conjunto) string = TX_ALLOC(struct conjunto, sizeof(struct conjunto));
    memcpy(D_RW(string), &buf, sizeof(struct conjunto));

    // snapshot before changing
    TX_ADD(root);

    D_RW(root)->my_string = string;

  } TX_END


  pmemobj_close(pop);

  return 0;
}
