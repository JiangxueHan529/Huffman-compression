/*
Name: Jiangxue Han
File:Tmain2.c
Desc: This is the file for puff the decompressor, it includes the huff_tree.h 
header file for the purpose of constructing the huffman tree. I have a main 
function to process information from the file, getting frequencies of characters
and build the tree, and functions to decode according to the tree and bitreader
function that reads chars and turn them into int arrays and traverse the tree to
decode write texts to puffed file
 */
#include<stdio.h>
#include<stdlib.h>
#include "huff_tree.h"
int* bitreader(char c);
int findChar(int* toWrite, huff_tree_node* head,int* numInt, int counter);
/*
@param int argc: number of arguments, should be 3
@param char* argv[]: command line arguments, should be 
executable, output.huff ,puffed.txt
 */
int main(int argc, char* argv[]){
if(argc != 3){
    printf("Inapropriate number of arguments\n");
    return -1;
  }
 FILE *fp;
  fp = fopen(argv[1], "r");
   if (fp == NULL) {
      printf("Unable to open file %s.\n", argv[1]);
      exit(EXIT_FAILURE);
   }
   int freqTable[128];
   for(int a = 0; a < 128;a++){
     freqTable[a] = 0;
   }
   //reading frequency table
   fread(freqTable,sizeof(int),128,fp);
   //building the heap,same as in Tmain.c
   huff_tree_node* myHeap[128];
   int size = 0;
   for(int a = 0; a < 128; a++){
     if(freqTable[a] != 0){
       huff_tree_node* aNode = createNode(a,freqTable[a]);
       heapInsert(myHeap,size,aNode);
       size++;
     }
   }

  FILE *fout;
  fout = fopen(argv[2], "w");
   int* HuffTable[128];
   int count = size;
   int namecounter = -1;
   int HuffSize[128];
   for(int h = 0; h < 128;h++){
     HuffSize[h] = 0;
   }
   while(size > 1){
     huff_tree_node* x = heapPoll(myHeap,size);
     size--;
     huff_tree_node* y = heapPoll(myHeap,size);
     size--;
     huff_tree_node* n = createNode(namecounter, (x->freq) + (y->freq));
     namecounter--;
     n->left = x;
     n->right = y;
     heapInsert(myHeap,size,n);
      size++;
   }
   
   huff_tree_node* head = myHeap[0];
   char c;
   int toWrite[50];
   int bitptr = 0;
   while(1){
     c= fgetc(fp);
     //use feof() because char '\377' is also interpreted as EOF
     if(feof(fp)){
       break;
     }
     int* myarr = bitreader(c);// the int arr
     for(int j = 0; j < 8; j++){
       toWrite[bitptr++] = myarr[j];
     }
     free(myarr);
     //numInt is used to keep track of how many digits have been processed
     int numInt = 1;
     while(numInt > 0){
       numInt = 0;
     int myChar = findChar(toWrite, head, &numInt, bitptr);
       char thisChar = (char)myChar;
       if(numInt == 0){
	 break;
       }
       else if(numInt == -1){
       break;
       } //writing to puffed.txt
       fputc(thisChar, fout);
       for(int q = numInt; q < 50; q++){
	 toWrite[q - numInt] = toWrite[q];
       }
       bitptr -= numInt;
     }
     if(numInt == -1){
       break;
     } 
   }
   fclose(fp);
   fclose(fout); 
}

/*
@param char c: the char read as input
@return int* : return an int array as bits
This function reads a char and returns the 
numerical representation of the bits in an 
int arr
 */
int* bitreader(char c){
  int* bitarr = malloc(sizeof(int) * 8);
  int mask = 1;
  int digit;
  for(int i = 0; i < 8; i++){
    digit = mask & c >> (7-i);
    *bitarr = digit;
    bitarr++;
  }
  bitarr -= 8;
  return bitarr;
}
/*
@param int* toWrite: arr of bits
@param huff_tree_node* head: current node processed
@param int* numInt: number of digits processed
@param int counter: how many digits are left
This function finds the char represented by bits.
if unsuccessful, returns '\0' and set numInt to 
specific values to alert
 */
int findChar(int* toWrite, huff_tree_node* head, int* numInt, int counter){
  //case where it's not finished, needs to read next char to decide
  if(counter == 0){
    *numInt = 0;
    return '\0';
  }
  else{
    //case where encoding is wrong
    if(*toWrite == 0){
      if(head->left == NULL){
	*numInt = -1;
	return '\0';
      }
      else{
	if(head->left->left == NULL){
	  *numInt = *numInt + 1;
	  return head->left->ch;
	}
	else{
	  toWrite++;
	  *numInt = *numInt + 1;
	  counter--;
	  return findChar(toWrite, head->left, numInt, counter);
	}
      }
    
    }
    else{
      if(head->right == NULL){
	*numInt = -1;
	return '\0';
      }
      else{
	if(head->right->left == NULL){
	   *numInt = *numInt + 1;
	  return head->right->ch;
	}
	else{
	  toWrite++;
	  *numInt = *numInt + 1;
	  counter--;
	  return findChar(toWrite, head->right, numInt, counter);
	}
      }
    }
  }
}
