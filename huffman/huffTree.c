#include "huffTree.h"
#include "list.h"

struct _HuffTree
{
  unsigned char byte;
  long long int frequency;
  HuffTree_t *next;
  HuffTree_t *left;
  HuffTree_t *right;
};

HuffTree_t* createNode(unsigned char byte, long long int frequency)
{
  HuffTree_t *temp = (HuffTree_t*) malloc(1 * sizeof(HuffTree_t));
  temp->byte = byte;
  temp->frequency = frequency;
  temp->next = NULL;
  temp->left = NULL;
  temp->right = NULL;
  return(temp);
}

HuffTree_t* createTree(unsigned char byte, long long int frequency, HuffTree_t *left, HuffTree_t *right)
{
  HuffTree_t *temp = (HuffTree_t*) malloc(1 * sizeof(HuffTree_t));
  temp->byte = byte;
  temp->frequency = frequency;

  temp->next = NULL;

  temp->left = left;
  if (left != NULL)
    left->next = NULL;

  temp->right = right;
  if (right != NULL)
    right->next = NULL;

  return(temp);
}

void createTreeFromPreFix(FILE *pFile, HuffTree_t **newTree, int end, int *i)
{
  if ((*i) == end) return;

  unsigned char expression;
  if (fscanf(pFile, "%c", &expression) && expression == '*')
  {
    (*newTree) = createNode('*', 0);
    (*i) ++; createTreeFromPreFix(pFile, &(*newTree)->left, end, &*i);
    (*i) ++; createTreeFromPreFix(pFile, &(*newTree)->right, end, &*i);
  }
  else
  {
    if (expression == 92)
    {
      int aux = expression;
      fscanf(pFile, "%c", &expression);
      if (expression == '*' || expression == 92)
        (*newTree) = createNode(expression, 0);
      else
        (*newTree) = createNode(aux, 0);
    }
    else
      (*newTree) = createNode(expression, 0);
  }
}

HuffTree_t* createTreeFromFile(char pathFile[])
{
  FILE *pFile;
  pFile = fopen(pathFile, "rb");
  if (pFile == NULL)
  {
    printf("We weren't able to find this file!\n");
    return(NULL);
  }

  long long int freq[256]; int i;
  for (i = 0; i < 256; i ++)
    freq[i] = 0;

  unsigned char bytes;
  long long int progress = 0; int atual = 0;
  long long int *progressBar = createProgressBar(pFile);
  while (fscanf(pFile, "%c", &bytes) != EOF)
  {
    freq[bytes] ++; // contagem de char no lugar certo!
    if (progressBar[atual] == progress)
      updateProgress("Creating Tree.........\0", atual ++, true);

    progress ++;
  }

  if (progress == 0)
  {
    printf("This file can't be compressed.\n");
    return(NULL);
  }

  fclose(pFile);

  List_t *list = createListFromArray(freq);

  sortList(list);

  HuffTree_t *compressedTree = createTreeFromList(list);

  free(progressBar);
  destroyList(list);
  return(compressedTree);
}

bool isHuffTreeEmpty(HuffTree_t *hm)
{
  return(hm == NULL);
}

bool isMoreFrequent(HuffTree_t *a, HuffTree_t *b)
{
  return(a->frequency > b->frequency);
}

void swapHuffTrees(HuffTree_t *a, HuffTree_t *b)
{
  HuffTree_t *aux = (HuffTree_t*) malloc(1 * sizeof(HuffTree_t));
  aux->byte = a->byte;
  a->byte = b->byte;
  b->byte = aux->byte;

  aux->frequency = a->frequency;
  a->frequency = b->frequency;
  b->frequency = aux->frequency;

  free(aux);
}

HuffTree_t* getLeft(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return(NULL);

  return(atual->left);
}

HuffTree_t* getRight(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return(NULL);

  return(atual->right);
}

unsigned char getByte(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return(0);

  return(atual->byte);
}

HuffTree_t* getNext(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return(NULL);

  return(atual->next);
}

long long int getFrequency(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return(INT_MIN);

  return(atual->frequency);
}

void setNext(HuffTree_t *atual, HuffTree_t *next)
{
  if (isHuffTreeEmpty(atual)) return;

  atual->next = next;
}

void printNode(HuffTree_t *atual)
{
  if (isHuffTreeEmpty(atual)) return;

  printf("Letra: %c || Frequencia: %lld\n", atual->byte, atual->frequency);
}

void printTreeInFile(FILE *newFile, HuffTree_t *tree)
{
  if (!isHuffTreeEmpty(tree))
  {
    if (isHuffTreeEmpty(tree->left) && isHuffTreeEmpty(tree->right) && (tree->byte == '*' || tree->byte == 92))
      fprintf(newFile, "%c", 92);

    fprintf(newFile, "%c", tree->byte);
    printTreeInFile(newFile, tree->left);
    printTreeInFile(newFile, tree->right);
  }
}

int countTrashSize(int bitsQuantity[], long long int frequency[])
{
	int i; long long int trashSize = 0;
	for(i = 0; i < 256; i ++)
		if(frequency[i] != -1)
			trashSize += (bitsQuantity[i] * frequency[i]) % 8;

	trashSize = 8 - (trashSize % 8);
  return(trashSize);
}

int countTreeSize(HuffTree_t *tree)
{
  int sum = 0;
  if (!isHuffTreeEmpty(tree))
  {
    if (isHuffTreeEmpty(tree->left) && isHuffTreeEmpty(tree->right) && (tree->byte == '*' || tree->byte == 92))
      sum = 1;

    sum += 1 + countTreeSize(tree->left);
    sum += countTreeSize(tree->right);
  }
  return(sum);
}

void destroyHuffTree(HuffTree_t *tree)
{
  if (!isHuffTreeEmpty(tree))
  {
    destroyHuffTree(tree->left);
    destroyHuffTree(tree->right);
    free(tree);
  }
}
