all: Tmain Tmain2
Tmain2: Tmain2.o huff_tree.o
	gcc -g -o puff Tmain2.o huff_tree.o

Tmain: Tmain.o huff_tree.o
	gcc -g  -o huff Tmain.o huff_tree.o

Tmain.o: huff_tree.h Tmain.c
	gcc  -g -c Tmain.c

Tmain2.o: huff_tree.h Tmain2.c
	gcc -g -c Tmain2.c

huff_tree.o: huff_tree.h huff_tree.c
	gcc  -g -c huff_tree.c
clean:
	-rm -f *~ *.o     


