#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libpmemobj.h>
#include <locale.h>

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

static void print_todos(const TOID(struct entry) str)
{
	const struct entry *aux= D_RO(str);
	printf("Numero: %d\n", aux->valor);
	printf("String: %s\n", aux->data);
	if (!TOID_IS_NULL(D_RO(str)->next))
	{
		print_todos(D_RO(str)->next);	//Utiliza recursão para printar todos
	}
	return;
}

void print_atual(TOID(struct entry) noh_atual){
	const struct entry *aux= D_RO(noh_atual);	//Printa apenas 1
	printf("Numero: %d\n", aux->valor);
	printf("String: %s\n", aux->data);
	return;
}

TOID(struct root) insere_final(PMEMobjpool *pop,const char *data,int valor,TOID(struct root) root){
	if(!TOID_IS_NULL(D_RO(root)->head)){			//Testa se a lista não esta vazia
		TOID(struct entry) aux=(D_RO(root)->head);
		while(!TOID_IS_NULL(D_RO(aux)->next))
		{
			aux=D_RO(aux)->next;			//Vai até o ultimo elemento
		}
		TX_BEGIN(pop) {
			/* now we can safely allocate and initialize the new entry */
			TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(data)+1);
			D_RW(entry)->valor = valor;
			D_RW(entry)->next=D_RO(aux)->next;
			memcpy(D_RW(entry)->data, data, strlen(data)+1);

			// snapshot before changing
			TX_ADD(aux);
			D_RW(aux)->next = entry;
		} TX_END
	}
	else{			//Se a lista esta vazia, coloca na cabeça
		TX_BEGIN(pop) {
			/* now we can safely allocate and initialize the new entry */
			TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(data)+1);
			D_RW(entry)->valor = valor;
			D_RW(entry)->next=D_RO(root)->head;
			memcpy(D_RW(entry)->data, data, strlen(data)+1);	

			// snapshot before changing
			TX_ADD(root);
			D_RW(root)->head = entry;
		} TX_END
	}
	return root;


}


TOID(struct root) remove_cabeca(PMEMobjpool *pop,TOID(struct root) root){
	TOID(struct entry) aux;
	aux=D_RO(root)->head;
	TX_BEGIN(pop){
		TX_ADD(root);
		D_RW(root)->head = D_RO(aux)->next;	//Aponta para o próximo e remove a aantiga cabeça
		TX_FREE(aux);

	}TX_END
	return root;
}


TOID(struct root) insere_cabeca(PMEMobjpool *pop,const char *data,int valor,TOID(struct root) root){
	TX_BEGIN(pop) {
		/* now we can safely allocate and initialize the new entry */
		TOID(struct entry) entry = TX_ALLOC(struct entry,sizeof(struct entry) + strlen(data)+1);
		D_RW(entry)->valor = valor;
		D_RW(entry)->next=D_RO(root)->head;
		memcpy(D_RW(entry)->data, data, strlen(data)+1);

		// snapshot before changing
		TX_ADD(root);
		D_RW(root)->head = entry;
	} TX_END	
	return root;
}

void remove_noh(PMEMobjpool *pop,TOID(struct entry) noh_anterior){
	TOID(struct entry) noh_atual=D_RO(noh_anterior)->next;
	if(D_RO(noh_atual)==D_RO(noh_anterior)){
		printf("Nó atual é a cabeça!\n");
		return;
	}
	TX_BEGIN(pop){
		TX_ADD(noh_anterior);
		D_RW(noh_anterior)->next=D_RO(noh_atual)->next; //Nó anterior aponta para o próximo nó do atual, e remove o nó atual
		TX_FREE(noh_atual);
	
	
	}TX_END
	return;

}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Portuguese");
	if (argc != 2) {
		printf("usage: %s file-name\n", argv[0]);
		return 1;
	}

	PMEMobjpool *pop = pmemobj_open(argv[1], LAYOUT_NAME);
	if (pop == NULL) {
		perror("pmemobj_open");
		return 1;
 	}
 	int opc=0;
 	int num=0;
	char str[100]={0};
 	TOID(struct root) root = POBJ_ROOT(pop, struct root);
 	TOID(struct entry) noh_atual=D_RO(root)->head;
 	TOID(struct entry) noh_anterior=D_RO(root)->head;
 	while(1){
 		printf("\n\n0->Imprimir nó atual\n1-> Imprimir todos\n2->Próximo nó\n3->Inserir nova cabeça\n4->Inserir no final\n5->Remover cabeça\n6->Remover nó atual\n7->Nó atual volta para início\n8->Sair\nOpção: ");
 		scanf("%d",&opc);
 		switch(opc){
 			case 0:
 				system("clear");
 				print_atual(noh_atual);
 				break;
 			case 1:
 				system("clear");
				if (!TOID_IS_NULL(D_RO(root)->head))
				{
					print_todos(D_RO(root)->head);
				}
				else{
					printf("\nVazio");
				}
				break;
				
			case 2:
				system("clear");
				noh_anterior=noh_atual;
				noh_atual=D_RO(noh_atual)->next;
				break;
				
			case 3:
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
				root=(insere_cabeca(pop,str,num,root));
				if(D_RO(noh_anterior)==D_RO(noh_atual)){
					noh_anterior=D_RO(root)->head;
					noh_atual=D_RO(root)->head;
				}
				break;
				
			case 4:
				system("clear");
				printf("String: ");
				if (scanf("%s", str) == EOF) {
					fprintf(stderr, "EOF\n");
					return 1;
				}
				printf("Numero: ");
				if (scanf("%d", &num) == EOF) {
					fprintf(stderr, "EOF\n");
					return 1;
				}
				root=(insere_final(pop,str,num,root));
				break;
				
			case 5:
				system("clear");
				root=remove_cabeca(pop,root);
				if(D_RO(noh_anterior)==D_RO(noh_atual)){
					noh_anterior=D_RO(root)->head;
					noh_atual=D_RO(root)->head;
				}
				printf("Removido\n");
				break;
				
			case 6:
				system("clear");
				printf("Removido");
				remove_noh(pop,noh_anterior);
				noh_atual=D_RO(noh_anterior)->next;
				break;
				
			case 7:
				system("clear");
				noh_anterior=D_RO(root)->head;
				noh_atual=D_RO(root)->head;
				break;
				
			case 8:
				return 0;
		}
	}

	return 0;


}
