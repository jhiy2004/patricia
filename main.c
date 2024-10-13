#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 51

typedef struct patricia_node{
	int bit; // Posição do bit que discrimina este nó
	unsigned char *key; // Chave armazenada no nó, NULL para nós intermediários
	struct patricia_node *left; //Subárvore esquerda
	struct patricia_node *right; //Subárvore direita
}PatriciaNode;

void freeNode(PatriciaNode* root){
	free(root->key);
	free(root);
}

unsigned bit(unsigned char *key, int k){
	int cont=0;
	while(key[cont] != '\0'){
		cont++;
	}

	if(cont*8 < k){
		return 0;
	}

	int pos = k / 8;
	int offset = k % 8;

	return key[pos] >> (sizeof(unsigned char)*8 - 1 - offset) & 1;
}

void initialize(PatriciaNode** root){
	(*root)->key = (unsigned char*)malloc(sizeof(unsigned char) * MAX);
	(*root)->left = *root;
	(*root)->right = *root;
	(*root)->bit = -1;

	for(int i=0; i < MAX; i++)
	{
		(*root)->key[i] = UCHAR_MAX;
	}
}

PatriciaNode* rec_search(PatriciaNode* root, unsigned char* x, int w){
	if(root->bit <= w)
		return root;

	if(bit(x, root->bit) == 0)
		return rec_search(root->left, x, root->bit);
	else
		return rec_search(root->right, x, root->bit);
}

PatriciaNode* search(PatriciaNode* root, unsigned char* x){
	PatriciaNode* t = rec_search(root->left, x, -1);
	int i = 0;
	while(x[i] != '\0')
	{
		if(x[i] != t->key[i])
			return NULL;
		i++;
	}
	return t;
}

PatriciaNode* rec_insertion(PatriciaNode* root, unsigned char* key, int w, PatriciaNode* father){
	PatriciaNode* new;
	int keySize = 0;
	if((root->bit >= w) || (root->bit <= father->bit)){
		new = (PatriciaNode*)malloc(sizeof(PatriciaNode));

		while(key[keySize] != '\0')
		{
			keySize++;
		}
		new->key = (unsigned char*)malloc(sizeof(unsigned char)*keySize);
		strcpy(new->key, key);

		new->bit = w;

		if(bit(key, new->bit) == 1){
			new->left = root;
			new->right = new;
		}else{
			new->left = new;
			new->right = root;
		}
		return new;
	}
	if(bit(key, root->bit) == 0)
		root->left = rec_insertion(root->left, key, w, root);
	else
		root->right = rec_insertion(root->right, key, w, root);
	return root;
}

void insertion(PatriciaNode **root, unsigned char* key){
	int i=0;
	PatriciaNode* t = rec_search((*root)->left, key, -1);
	if(key == t->key)
		return;
	int tSize = 0;
	int keySize = 0;

	while(key[keySize] != '\0')
		keySize++;
	
	while(t->key[tSize] != '\0')
		tSize++;

	while(bit(key, i) == bit(t->key, i)){
		i++;
	}
	(*root)->left = rec_insertion((*root)->left, key, i, *root);
}

PatriciaNode* findBackPointer(PatriciaNode* root){
	PatriciaNode* aux = NULL;
	PatriciaNode* q = NULL;

	if(bit(root->key, root->bit) == 0){
		q = root->left;
	}else{
		q = root->right;
	}

	if(q == root){
		return q;
	}

	while(strcmp(q->key, root->key) != 0){
		aux = q;
		if(bit(root->key,q->bit) == 0){
			q = q->left;
		}else{
			q = q->right;
		}
	}
	return aux;
}

void deletion(PatriciaNode* root, unsigned char* key)
{
	PatriciaNode* p = root->left;
	PatriciaNode* q = root;

	int w = root->bit;

	while(strcmp(p->key, key) != 0){
		if(p->bit <= w){
			break;
		}

		q = p;
		w = p->bit;

		if(bit(key,p->bit) == 0){
			p = p->left;
		}else{
			p = p->right;
		}
	}

	if(strcmp(p->key, key) != 0) {
		printf("Not founded\n");
		return;
	}

	//Have one self pointer
	if(p->left == p || p->right == p){

		if(p->left == p){
			//Self pointer left
			if(q->right == p){
				q->right = p->right;
			}else{
				q->left = p->right;
			}
		}else{
			//Self pointer right
			if(q->right == p){
				q->right = p->left;
			}else{
				q->left = p->left;
			}
		}

		freeNode(p);

		return;
	}
	
	//Have zero self pointer
	PatriciaNode* r = NULL;
	PatriciaNode* parent = root->left;
	PatriciaNode* aux = root;

	//Put in q a pointer whose points back to p
	q = findBackPointer(p);
	r = findBackPointer(q);

	//Find the parent of q
	while(strcmp(parent->key, q->key) != 0){
		aux = parent;

		if(bit(q->key, parent->bit) == 0){
			parent = parent->left;
		}else{
			parent = parent->right;
		}
	}
	parent = aux;

	if(r->right == q){
		r->right = p;
	}else{
		r->left = p;
	}

	if(parent->left == q){
		if(q->left == p){
			parent->left = q->right;
		}else{
			parent->left = q->left;
		}
	}else{
		if(q->left == p){
			parent->right = q->right;
		}else{
			parent->right = q->left;
		}
	}

	char* temp = p->key;
	
	//Copy to p the value o q
	p->key = q->key;
	q->key = temp;

	freeNode(q);
}

void rec_print(PatriciaNode* root, int w){
	if(root->bit <= w){
		return;
	}

	int keySize=0;

	printf("Key: ");
	while(root->key[keySize] != '\0')
	{
		keySize++;
	}
	printf("(%s)", root->key);
	for(int k=0; k < keySize*8; k++)
	{
		if(k % 8 == 0 && k != 0)
			printf(" ");
		printf("%d", bit(root->key, k)); }
	printf("\n");

	printf("Bit: %d\n\n", root->bit);

	printf("Left: %s\n", root->left->key);
	rec_print(root->left, root->bit);

	printf("Right: %s\n", root->right->key);
	rec_print(root->right, root->bit);
}

void print(PatriciaNode* root){
	printf("Dummy Left: \n");
	rec_print(root->left, root->bit);
}

int menu(){
	int opc = 0;
	do{
		printf("=====================================\n");
		printf("=           Patricia Trie           =\n");
		printf("=====================================\n");
		printf("= 1 - Insert a new word (MAX=50)    =\n");
		printf("= 2 - Remove  word                  =\n");
		printf("= 3 - Print patricia trie           =\n");
		printf("= 4 - Exit                          =\n");
		printf("=====================================\n");
		printf("Enter your option: ");
		scanf("%d", &opc);
	}while(opc < 1 || opc > 4);

	return opc;
}

void pause(){
	printf("\nDigite qualquer tecla para prosseguir...");
	getchar();
	system("clear");
}

void test(PatriciaNode** root){
	char *words[] = {"opa", "opanda", "opanba", "obesilisco", "horta", "obra"};
	int lengthWords = 6;

	char delIndexes[] = {4, 1, 2};
	int lengthDelIndexes = 3;

	for(int i=0; i < lengthWords; i++){
		printf(">_ Inserindo '%s'\n", words[i]);
		insertion(root, words[i]);
		print(*root);

		pause();
	}

	for(int i=0; i < lengthDelIndexes; i++){
		printf(">_ Removendo '%s'\n", words[i]);
		deletion(*root, words[i]);
		print(*root);

		pause();
	}
}

int main(int argc, char *argv[]){
	int opc = 0;
	PatriciaNode* root = (PatriciaNode*)malloc(sizeof(PatriciaNode));
	char name[MAX];

	initialize(&root);

	if(argc > 1){
		if(strcmp(argv[1],"test") == 0){
			test(&root);
			return 0;
		}
	}

	while(1){
		opc = menu();

		getchar();

		switch(opc){
			case 1:
				printf("Enter the word to be inserted: ");

				fgets(name, MAX, stdin);
				name[strcspn(name, "\n")] = '\0';

				insertion(&root, name);
				break;

			case 2:
				printf("Enter the word to be deleted: ");

				fgets(name, MAX, stdin);
				name[strcspn(name, "\n")] = '\0';

				deletion(root, name);
				break;
			case 3:
				print(root);
				break;
			case 4:
				return 0;
				break;
		}
	}

	/*
	insertion(&root, "opa");
	insertion(&root, "opanda");
	insertion(&root, "opanba");
	insertion(&root, "obesilisco");
	insertion(&root, "horta");
	insertion(&root, "obra");

	deletion(root, "obesilisco");
	
	print(root);
	*/
	return 0;
}
