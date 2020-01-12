/*
Name: Jiangxue Han
File:Tmain.c
Desc: This is the file for huff the compressor, it includes the huff_tree.h 
header file for the purpose of constructing the huffman tree. I have a main 
function to process information from the file, getting frequencies of characters
and build the tree, and functions to encode according to the tree and bitwriter
function that writes 8 ints into a char and write it to the output file.
 */

#include<stdio.h>
#include<stdlib.h>
#include "huff_tree.h"
int  encode(int huffarr[],int*myHuffTable[],
	     huff_tree_node* curNode,int index1,int huffSize[],int** arrTable);
void  bitwriter(int arr[8], FILE* fout);
void treeFree(huff_tree_node* root);
/*
@param int argc: number of arguments, should be 3
@param char* argv[]: command line arguments, should be 
executable, input.txt, output.huff
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
    
   int table[128];
   for(int i = 0; i < 128; i++){
     table[i] = 0;
   }
   char c;
   char d;
   while(1){
     c = fgetc(fp);
     if(c == EOF){
       break;
     }
     //skip '\n' only if it's at the end
     if(c == '\n' && (d = fgetc(fp) == EOF)){
       fseek(fp, -1, SEEK_CUR);
       break;
     }
     table[c]++;
   }
   FILE *fout;
   fout = fopen(argv[2], "w");
   if (fout == NULL) {
      printf("Unable to open file %s.\n", argv[2]);
      exit(EXIT_FAILURE);
   }
   //writing the frequencies
    fwrite(table,sizeof(int),128,fout);
   huff_tree_node* myHeap[128];
   int size = 0;
   //creating min heap
   for(int a = 0; a < 128; a++){
     if(table[a] != 0){
       huff_tree_node* aNode = createNode(a,table[a]);
       heapInsert(myHeap,size,aNode);
       size++;
     }
   }
   int* HuffTable[128];
   int count = size;
   int namecounter = -1;
   int HuffSize[128];
   for(int h = 0; h < 128;h++){
     HuffSize[h] = 0;
   }
   //popping from heap to create tree
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
   huff_tree_node* curNode = myHeap[0];
   int huffarr[400];
   //the table to keep track of each char's encoding
   int** arrTable = malloc(1000*(sizeof(int*)));
   int numFree = encode(huffarr, HuffTable, curNode,0,HuffSize,arrTable);
    arrTable -= numFree;
   fclose(fp);
    FILE *fp2;
  fp2 = fopen(argv[1], "r");
   int bits[50];
   int bitptr = 0;
   while(1){
     c = fgetc(fp2);
     if(c == EOF){
       break;
     }
     if(c == '\n' && (d = fgetc(fp) == EOF)){
       fseek(fp2, -1, SEEK_CUR);
       break;
     }
    int* myBits = HuffTable[c];
    int num = HuffSize[c];
    for(int i = 0; i < num; i++){
      bits[bitptr] = myBits[i];
      bitptr++;
    }
     while(bitptr >= 8){
      int arr[8];
       for(int j = 0; j < 8; j++){
	 arr[j] = bits[j];
       }
       bitwriter(arr,fout);
     
       for(int e = 8; e <50;e++){
	 bits[e - 8] = bits[e];
     }
       bitptr -= 8;
   }
   }
   
   if(bitptr > 0){
     int maxSize = 0;
     int indexTracker = 0;
     for(int t = 0; t < 128; t++){
       if(HuffSize[t] > maxSize){
	 maxSize = HuffSize[t];
	 indexTracker = t;
     }
     }
     //deal with leftover bits, not perfect
     int* longestCode = HuffTable[indexTracker];
     if(longestCode[maxSize - 1] == 0){
       longestCode[maxSize - 1] = 1;
     }
     else{
      longestCode[maxSize - 1] = 0;
     }
     
     int arr2[8];
     for(int a = 0; a < bitptr; a++){
       arr2[a] = bits[a];
     }
     for(int b = bitptr; b < 8; b++){
       arr2[b] = longestCode[b - bitptr];
     }
     bitwriter(arr2,fout);
   }
   treeFree(curNode);
   fclose(fp2);
   fclose(fout);
   for(int r = 0; r < numFree; r++){
     free(arrTable[r]);
   }
   free(arrTable);
}
/*
@param int huffarr[]: the array to keep track of current place in the tree by 
adding 0 and 1s 
@param int* myHuffTable[] : the table that has encodings for all chars
@param  huff_tree_node* curNode:current node processed
@param int index1:number of digits currently in huffarr
@param int huffSize[]: table that keeps track of size of digits of encoding for 
each char
@param int** arrTable:table to hold encodings for all chars
@return int: used to keep track of how much pointer of arrTable has moved
This function is used to fill the arrTable of encodings of all characters by 
traversing the tree and put in each leaf.
 */
int  encode(int huffarr[],int* myHuffTable[],
	     huff_tree_node* curNode,int index1, int huffSize[],int** arrTable){
  if(curNode->left == NULL && curNode->right == NULL){
    myHuffTable[curNode->ch] = huffarr;
    huffSize[curNode->ch] = index1;
    return 0;
  }
  else{
    int* huffarr2 = malloc(sizeof(int)*(index1+1));
   
    for(int i = 0; i < index1; i++){
      huffarr2[i] = huffarr[i];
    }
    huffarr2[index1] = 0;
    int temp = index1;
    index1++;
    *arrTable = huffarr2;
    arrTable++;
   int numF =  encode(huffarr2, myHuffTable, curNode->left,index1,huffSize,arrTable);
    int* huffarr3 = malloc(sizeof(int)*(temp+1));
    
     for(int j = 0; j < temp; j++){
      huffarr3[j] = huffarr[j];
    }
    huffarr3[temp] = 1;
    temp++;
    *arrTable = huffarr3;
    arrTable++;
    return numF + encode(huffarr3, myHuffTable,curNode->right,temp,huffSize,arrTable);
  }
}
//little function to free nodes in the tree
void treeFree(huff_tree_node* root){
  if(root->left == NULL && root->right == NULL){
    free(root);
    return;
  }
  else{
    treeFree(root->left);
    treeFree(root->right);
    free(root);
  }
}
//little function that takes an array of integer and
//parse it bitwise into a char and write it to output file.
void bitwriter(int arr[8], FILE* fout){
  unsigned char c = 0;
  for(int i = 0; i < 8; i++){
    c |=   arr[i] << (7-i) ;
  }
   fwrite(&c,1,1,fout);
}
