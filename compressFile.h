#ifndef COMPRESSFILE_H
#define COMPRESSFILE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "huffTree.h"
#include "list.h"
#include "fileSetup.h"
#include "binaryOperations.h"

unsigned char* createHeader(int trashSize, int treeSize, huffTree_t *tree)
{
  unsigned char *bytes = (unsigned char*) malloc(3 * sizeof(unsigned char));
  bytes[0] = trashSize << 5 | treeSize >> 8;
  bytes[1] = treeSize;

  printf("Header: %c%c", bytes[0], bytes[1]);
  printTreePreOrder(tree); printf("\n");
  //printByte(bytes[0]);
  //printByte(bytes[1]);
  return(bytes);
}

void compressFile(FILE *pFile, unsigned char *header, unsigned int dictionary[][2], huffTree_t *tree)
{
  FILE *newFile = fopen("textoOut.txt", "wb");
  fprintf(newFile, "%c%c", header[0], header[1]);
  printTreeInFile(newFile, tree);

  unsigned char byte = 0, newByte = 0; int pos = 0; int needsToEnd = 0;
  while (fscanf(pFile, "%c", &byte) != EOF)
  {
    needsToEnd = 0;
    if (pos == 8)
    {
      fprintf(newFile, "%c", newByte);
      newByte = 0;
      pos = 0;
    }
    if (pos + dictionary[byte][1] >= 8)
    {
      printf("Then ended ");
      newByte |= dictionary[byte][0] >> (dictionary[byte][1] - (8 - pos));
      fprintf(newFile, "%c", newByte);
      printByte(newByte, 8); printf("=%c#%dHERE ", byte, pos);
      newByte = 0;
      needsToEnd = (8 - pos);
      if ((dictionary[byte][1] - (8 - pos)) > 8)
      {
          printf("\n\nWTF: %d\n\n", 8 - (dictionary[byte][1] - (8 - pos)));
        newByte |= dictionary[byte][0] >> (dictionary[byte][1] - 8 - needsToEnd);
        fprintf(newFile, "%c", newByte);
        printByte(newByte, 8); printf("=%c#%dCOF ", byte, pos);
        newByte = dictionary[byte][0] << (8 - (dictionary[byte][1] - 8 - needsToEnd));
        printf("Like This: ##"); printByte(newByte, 8); printf("|%d| ", needsToEnd);
        pos = dictionary[byte][1] - 8 - needsToEnd; //> 8 ? 8 : dictionary[byte][1];
      }
      else if (dictionary[byte][1] >= 8)
      {
        newByte |= dictionary[byte][0] << (8 - (dictionary[byte][1] - (8 - pos)));
        pos = (dictionary[byte][1] - (8 - pos));
        printf("|"); printByte(newByte, 13); printf("~%d| ", pos);
      }
      else
      {
        newByte |= dictionary[byte][0] << (8 - (dictionary[byte][1] - (8 - pos)));
        printByte(newByte, 8); printf("=%c#%d ", byte, pos);
        pos = (dictionary[byte][1] - (8 - pos));
      }
    }
    else
    {
      newByte |= dictionary[byte][0] << (8 - dictionary[byte][1] - pos);
      printByte(newByte, 8); printf("=%c#%d ", byte, pos);
      pos += dictionary[byte][1];
    }
  }

  if (header[0] >> 5 != 0)
  {
    fprintf(newFile, "%c", newByte);
    printByte(newByte, 8);
  }
  printf("\n");
  fclose(newFile);
}

#endif //COMPRESSFILE_H