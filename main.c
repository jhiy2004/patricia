#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define MAX 50

typedef struct patricia_node{
	int bit; // Posição do bit que discrimina este nó
	unsigned char *key; // Chave armazenada no nó, NULL para nós intermediários
	struct patricia_node *left; //Subárvore esquerda
	struct patricia_node *right; //Subárvore direita
}PatriciaNode;

unsigned bit(unsigned char *key, int k){
	int pos = k / 8;
	int offset = k - 8*pos;

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
		if(i >= tSize*8 || i >= keySize*8)
			break;
		i++;
	}
	(*root)->left = rec_insertion((*root)->left, key, i, *root);
}

void deletion(PatriciaNode* root, unsigned char* key)
{
	printf("Antes\n");
	PatriciaNode* t = search(root, key);
	printf("Depois\n");

	if(t == NULL)
		return;

	PatriciaNode* z = NULL;
	PatriciaNode* y = t;

	do
	{
		printf("Y-Key: %s\n", y->key);
		printf("Y-R-Key: %s\n", y->right->key);
		printf("Y-L-Key: %s\n", y->left->key);
		z = y;
		if(bit(key, y->bit) == 1)
			y = y->right;
		else
			y = y->left;
	}
	while(y != t);
	
	//Self-pointer
	if(t == z)
		return;

	int keySize = 0;
	printf("Z-Key: ");
	while(z->key[keySize] != '\0')
	{
		keySize++;
	}
	printf("(%s)", z->key);
	for(int k=0; k < keySize*8; k++)
	{
		if(k % 8 == 0 && k != 0)
			printf(" ");
		printf("%d", bit(z->key, k));
	}
	printf("\n");
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
		printf("%d", bit(root->key, k));
	}
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

int main(){
	PatriciaNode* root = (PatriciaNode*)malloc(sizeof(PatriciaNode));

	initialize(&root);

	insertion(&root, "opa");
	insertion(&root, "opanda");
	insertion(&root, "opanba");
	insertion(&root, "obesilisco");
	insertion(&root, "caule");
	insertion(&root, "antena");
	
	print(root);
	return 0;
}
