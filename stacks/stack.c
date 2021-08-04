#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libpmemobj.h>

/*
 Create the pool for this app with:
 pmempool create obj --layout=pm_stack <pool-name>
*/
#define LAYOUT_NAME "pm_stack"
POBJ_LAYOUT_BEGIN(stack);
POBJ_LAYOUT_ROOT(stack, struct root);
POBJ_LAYOUT_TOID(stack, struct entry);
POBJ_LAYOUT_END(stack);


struct entry{ /* queue entry that contains arbitrary data */
	TOID(struct entry) next;
	int valor; 
	char data[];
};

struct root{
	TOID(struct entry) head;
};


static void print_only(const TOID(struct entry) str)
{
	const struct entry *aux= D_RO(str);
	printf("Numero: %d\n", aux->valor);
	printf("String: %s\n", aux->data);
	return;
}

static void print_all(const TOID(struct entry) str)
{
	const struct entry *aux= D_RO(str);
	printf("Numero: %d\n", aux->valor);
	printf("String: %s\n", aux->data);
	if (!TOID_IS_NULL(D_RO(str)->next))
	{
		print_all(D_RO(str)->next);
	}
	return;
}

TOID(struct root) push(PMEMobjpool *pop,const char *data,int valor,TOID(struct root) root){
	TX_BEGIN(pop) {
		/* Abre uma janela segura para fazer a transação */
		TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(data)+1);
		D_RW(entry)->valor = valor;
		D_RW(entry)->next=D_RO(root)->head;
		memcpy(D_RW(entry)->data, data, strlen(data)+1);

		// Salva o estado de root antes de altera-lo
		TX_ADD(root);
		D_RW(root)->head = entry;
	} TX_END	
	return root;
}


TOID(struct root) stack_pop(PMEMobjpool *pop,TOID(struct root) root){
	TOID(struct entry) aux;
	aux=D_RO(root)->head;
	print_only(aux);
	TX_BEGIN(pop){
	/* Abre uma janela segura para fazer a transação */
	// Salva o estado de root antes de altera-lo
		TX_ADD(root);
		D_RW(root)->head = D_RO(aux)->next;
		TX_FREE(aux);

	}TX_END
	printf("Removido\n");
	return root;
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
		print_all(D_RO(root)->head);
	}
	int num=0;
	int opc=0;
	char str[100]={0};
	while(1){
 		printf("\n1->Push\n2->Pop\n3->List all\n4->Exit\nOption: ");
 		scanf("%d",&opc);
 		switch(opc){
 			case 1:
 				system("clear");
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
				root=push(pop,str,num,root);
				break;
				
			case 2:
				root=stack_pop(pop,root);
				break;
				
			case 3:
				system("clear");
				if (!TOID_IS_NULL(D_RO(root)->head))
				{
					print_all(D_RO(root)->head);
				}
				else{
					printf("\nVazio");
				}
				break;
				
			case 4:
				return 0;
			}


	}
	return 0;
}
