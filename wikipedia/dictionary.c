#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
		struct Node *next[256];
		unsigned long int number;
		} NODE;

typedef struct Term {
		unsigned long int wordid;
		struct Term *under;
		struct Term *next;
		} TERM;

void Find(unsigned char *word, NODE *dict)
{
	unsigned char *pt=word;
	NODE *node=dict;
	while((*pt!='\n') && (*pt!='\0') && (node->next[*pt]!=NULL))
		node=node->next[*pt++];
	if(((*pt=='\n') || (*pt=='\0')) && (node->number>0))
		printf(" word %s found at position %ld.\n", word, node->number);
	else
		printf(" word %s was not found.\n",word);	
}

void Blank(NODE *node)
{
	int i;
	if(node!=NULL)
		for(i=0; i<=255; i++)
			node->next[i]=NULL;	
}

void main(int argc, char *argv[])
{
	NODE dict, *cnode=&dict;
	TERM thes, *tnode=&thes;
	unsigned char cchar;
	unsigned long int nbwords=0;
	unsigned long bytecount=0;
	int i;
	FILE *DictFile;
	unsigned char word[256];

	if(argc !=2)
	{
		fprintf(stderr, "Usage : %s Dictionary_file\n", argv[0]);
		exit(-1);
	}
	if((DictFile=fopen(argv[1], "r"))==NULL)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		exit(-1);
	}	
	Blank(cnode);
	while(!feof(DictFile))
	{
		cchar=fgetc(DictFile);
		if(cchar=='\n'||cchar==' ')
		{
			if(nbwords%100000==0)
				printf("Dictionary loaded with %ld words (%ld Mb)\n", nbwords, bytecount/1024/1024);

			cnode->number=++nbwords;
			cnode=&dict;
		}
		else if(cnode->next[cchar]==NULL)
		{
			cnode->next[cchar]=(NODE *) malloc(sizeof(NODE));
			bytecount+=sizeof(NODE);
			Blank(cnode->next[cchar]);
			cnode=cnode->next[cchar];
			cnode->number=0; 
		}
		else
			cnode=cnode->next[cchar];

		
	}
	fclose(DictFile);
	printf("Dictionary loaded with %ld words (%ld Mb)\n", nbwords, bytecount/1024/1024);
	while(!feof(stdin))
	{
		scanf("%s", word);
		Find(word, &dict);
	}
}
		
