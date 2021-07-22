#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libpmemobj.h>

/*
 Create the pool for this app with:
 pmempool create obj --layout=structdyn <pool-name>
*/
#define LAYOUT_NAME "structdyn"
POBJ_LAYOUT_BEGIN(queue);
POBJ_LAYOUT_ROOT(queue, struct root);
POBJ_LAYOUT_TOID(queue, struct entry);
POBJ_LAYOUT_END(queue);


struct entry{ /* queue entry that contains arbitrary data */
	TOID(struct entry) next;
	int valor; 
	char data[];
};

struct root{
	TOID(struct entry) head;
};


static void string_print(const TOID(struct entry) str)
{
	const struct entry *aux= D_RO(str);
	printf("Numero: %d\n", aux->valor);
	printf("String: %s\n", aux->data);
	if (!TOID_IS_NULL(D_RO(str)->next))
	{
		string_print(D_RO(str)->next);
	}
	return;
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
 	TOID(struct root) root = POBJ_ROOT(pop, struct root);
	if (!TOID_IS_NULL(D_RO(root)->head))
	{
		string_print(D_RO(root)->head);
	}
	int num=0;
	char str[100]={0};
	TOID(struct entry) aux=(D_RO(root)->head);
	printf(" \nString: ");
	if (scanf("%s", str) == EOF) {
		fprintf(stderr, "EOF\n");
		return 1;
	}
	printf("Numero: ");
	if (scanf("%d", &num) == EOF) {
		fprintf(stderr, "EOF\n");
		return 1;
	}
	if(!TOID_IS_NULL(D_RO(root)->head)){
		while(!TOID_IS_NULL(D_RO(aux)->next))
		{
			aux=D_RO(aux)->next;
		}
		TX_BEGIN(pop) {
			/* now we can safely allocate and initialize the new entry */
			TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(str)+1);
			D_RW(entry)->valor = num;
			D_RW(entry)->next=D_RO(aux)->next;
			memcpy(D_RW(entry)->data, str, strlen(str)+1);

			// snapshot before changing
			TX_ADD(aux);
			D_RW(aux)->next = entry;
		} TX_END
	}
	else{
		TX_BEGIN(pop) {
			/* now we can safely allocate and initialize the new entry */
			TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(str)+1);
			D_RW(entry)->valor = num;
			D_RW(entry)->next=D_RO(root)->head;
			memcpy(D_RW(entry)->data, str, strlen(str)+1);

			// snapshot before changing
			TX_ADD(root);
			D_RW(root)->head = entry;
		} TX_END
	}

	return 0;


}
