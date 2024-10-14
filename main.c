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

void freeNode(PatriciaNode* root){
	// Função para liberar um nó da memória
	free(root->key);
	free(root);
}

unsigned bit(unsigned char *key, int k){
	// Função para pegar o bit na posição k de um string
	
	// Conta o número de caracteres da string
	int cont=0;
	while(key[cont] != '\0'){
		cont++;
	}

	// Caso o o número de bits na string seja menor que k retorne 0
	if(cont*8 < k){
		return 0;
	}

	// pos armazena o indíce do caractere em que o bit k está localizado na string
	int pos = k / 8;
	// offset armazena qual o deslocamento de k em relação a pos
	int offset = k % 8;

	return key[pos] >> (sizeof(unsigned char)*8 - 1 - offset) & 1;
}

void initialize(PatriciaNode** root){
	// Função para inicializar a árvore patricia com uma string de 50 caracteres em que todos os bits são 1
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
	//Função para encontrar um nó na árvore patricia
	if(root->bit <= w)
		return root;

	if(bit(x, root->bit) == 0)
		return rec_search(root->left, x, root->bit);
	else
		return rec_search(root->right, x, root->bit);
}

PatriciaNode* search(PatriciaNode* root, unsigned char* x){
	//Função que informa se o nó encontrado em rec_search realmente era o que se
	//procurava, caso for retorna ele, caso contrário NULL
	PatriciaNode* t = rec_search(root->left, x, -1);
	int i = 0;
	while(x[i] != '\0')
	{
		if(x[i] != t->key[i])
			return NULL;
		i++;
	}

	//Caso x seja menor do que t retorna NULL, pois são diferentes
	if(t->key[i] != '\0') 
		return NULL;

	return t;
}

PatriciaNode* rec_insertion(PatriciaNode* root, unsigned char* key, int w, PatriciaNode* father){
	//Função para inserir um nó na árvore patricia
	PatriciaNode* new;
	int keySize = 0;
	
	//Caso tenha descido mais na árvore do que o nó com o prefixo mais 
	//semelhante, ou caso tenha subido um nó em relação a antiga recursão, insere o novo nó
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

int insertion(PatriciaNode **root, unsigned char* key){
	int i=0;
	int keySize = 0;
	int tSize = 0;

	while(key[keySize] != '\0')
		keySize++;

	if(keySize > MAX){
		return 0;
	}
	//Faz uma busca para encontrar o nó mais próximo de key
	PatriciaNode* t = rec_search((*root)->left, key, -1);

	//Caso tenha encontrado key na árvore, retorna o controle de volta ao chamador
	if(strcmp(key, t->key) == 0)
		return 0;

	while(t->key[tSize] != '\0')
		tSize++;

	//Após esse while, o i passa a armazenar o primeiro bit diferente entre key e a chave
	//do índice mais semelhante
	while(bit(key, i) == bit(t->key, i)){
		i++;
	}
	(*root)->left = rec_insertion((*root)->left, key, i, *root);
	return 1;
}

PatriciaNode* findBackPointer(PatriciaNode* root){
	//Função para encontrar o nó abaixo de root que aponta de volta para root
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

int deletion(PatriciaNode* root, unsigned char* key){
	//p é o nó que contém a chave a ser removida
	PatriciaNode* p = root->left;
	PatriciaNode* q = root;

	int w = root->bit;

	//Após esse while o q passa a ser o nó pai de p
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
		return 0;
	}

	//Tem um ponteiro que aponta para si mesmo
	if(p->left == p || p->right == p){

		if(p->left == p){
			//Ponteiro que aponta para si mesmo está na esquerda
			if(q->right == p){
				q->right = p->right;
			}else{
				q->left = p->right;
			}
		}else{
			//Ponteiro que aponta para si mesmo está na direita
			if(q->right == p){
				q->right = p->left;
			}else{
				q->left = p->left;
			}
		}

		// Remove o nó p
		freeNode(p);

		return 1;
	}
	
	//Não há ponteiro que aponta para si mesmo
	PatriciaNode* r = NULL;
	PatriciaNode* parent = root->left;
	PatriciaNode* aux = root;

	//q aramazena um ponteiro que aponta de volta para p
	q = findBackPointer(p);

	//r armazena um ponteiro que aponta de volta para q
	r = findBackPointer(q);

	//Encontra o nó pai de q
	while(strcmp(parent->key, q->key) != 0){
		aux = parent;

		if(bit(q->key, parent->bit) == 0){
			parent = parent->left;
		}else{
			parent = parent->right;
		}
	}
	parent = aux;

	// Remove o ponteiro de r que apontava para q
	if(r->right == q){
		r->right = p;
	}else{
		r->left = p;
	}

	// troca o ponteiro de parent que apontava para q, para apontar
	// para o ponteiro de q que não aponta de volta para p
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
	
	//Copia para p o valor de q
	p->key = q->key;
	q->key = temp;

	//Remove da memória o nó q
	freeNode(q);

	return 1;
}
// Função para imprimir a árvore
void rec_print(PatriciaNode *root, int level, int w) {
	if(root != NULL && root->bit > w) {
		rec_print(root->right, level+1, root->bit);
		printf("%*s-> %s | %d\n", level*5, "", root->key, root->bit);
		rec_print(root->left, level+1, root->bit);
	}
}

void print(PatriciaNode* root){
	if(root->left == root){
		printf("Empty\n");
		return;
	}
	rec_print(root->left, 0, root->bit);
}

void det_rec_print(PatriciaNode* root, int w){
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
	det_rec_print(root->left, root->bit);

	printf("Right: %s\n", root->right->key);
	det_rec_print(root->right, root->bit);
}
void det_print(PatriciaNode* root){
	printf("Dummy Left: \n");
	det_rec_print(root->left, root->bit);
}

int menu(){
	int opc = 0;
	do{
		printf("=====================================\n");
		printf("=           Patricia Trie           =\n");
		printf("=====================================\n");
		printf("= 1 - Insert a new word (MAX=50)    =\n");
		printf("= 2 - Remove a word                 =\n");
		printf("= 3 - Search for a word             =\n");
		printf("= 4 - Print patricia trie           =\n");
		printf("= 5 - Exit                          =\n");
		printf("=====================================\n");
		printf("Enter your option: ");
		
		if(scanf("%d", &opc) == 0){
			while(getchar() != '\n');
			opc = 0;
		}
	}while(opc < 1 || opc > 5);

	return opc;
}

void pause(){
	printf("\nDigite qualquer tecla para prosseguir...");
	getchar();
	system("clear");
}

void test(PatriciaNode** root){
	// Lista de palavras contidas na árvore patricia
	char *words[] = {"cachorro", "gato", "pato", "pavao", "cachorrinho", "gato2", "gato3", "gato4", "cachorrinho2"};
	int lengthWords = 9;

	// Lista de índices referentes a palavras do vetor words que serão removidas da árvore patricia
	char delIndexes[] = {1, 0, 5, 6, 3, 8};
	int lengthDelIndexes = 6;

	for(int i=0; i < lengthWords; i++){
		printf(">_ Inserindo '%s'\n", words[i]);

		printf("Antes:\n");
		print(*root);
		printf("\n");

		insertion(root, words[i]);

		printf("Depois:\n");
		print(*root);

		pause();
	}

	for(int i=0; i < lengthDelIndexes; i++){
		printf(">_ Removendo '%s'\n", words[i]);

		printf("Antes:\n");
		print(*root);
		printf("\n");

		deletion(*root, words[i]);

		printf("Depois:\n");
		print(*root);

		pause();
	}
}

int main(int argc, char *argv[]){
	int opc = 0;
	PatriciaNode* root = (PatriciaNode*)malloc(sizeof(PatriciaNode));
	char name[1000];

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

				fgets(name, 1000, stdin);
				name[strcspn(name, "\n")] = '\0';

				if(insertion(&root, name) == 1){
					printf("'%s' inserted", name);
				}else{
					printf("'%s' not inserted",name);
				}
				break;

			case 2:
				printf("Enter the word to be deleted: ");

				fgets(name, 1000, stdin);
				name[strcspn(name, "\n")] = '\0';

				if(deletion(root, name) == 1){
					printf("'%s' deleted", name);
				}else{
					printf("'%s' not deleted",name);
				}
				break;
			case 3:
				printf("Enter the word to be searched: ");

				fgets(name, 1000, stdin);
				name[strcspn(name, "\n")] = '\0';

				if(search(root,name) != NULL){
					printf("Founded\n");
				}else{
					printf("Not founded\n");
				}
				break;
			case 4:
				print(root);
				break;
			case 5:
				return 0;
				break;
		}
		pause();
	}
	return 0;
}
