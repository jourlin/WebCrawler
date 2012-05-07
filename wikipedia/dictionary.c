#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORDS 	2000000
#define TRUE 		1
#define FALSE		0
#define LINEMAXLENGTH	500

#define TRUE 1
#define FALSE 0

typedef struct Node {
		struct Node *next[256];
		unsigned long int number;
		} NODE;

typedef struct Term {
		struct Term *next; /* next word in term */
		struct Term *alter; /* next existing alternative */
		unsigned long int wordid;
		unsigned short int isfinal;
		} TERM;

unsigned long bytecount=0;

TERM* FindOrCreateNextAlternative(TERM *current, unsigned long int wordid)
{
	
	if(current->next==NULL) /* A new next alternative */
	{
		current->next=(TERM *) malloc(sizeof(TERM));
		bytecount+=sizeof(TERM);
		current=current->next;
		current->wordid=wordid;
		current->next=NULL;
		current->alter=NULL;
		current->isfinal=FALSE;
		return current;
	}
	current=current->next;
	while(current->alter!=NULL && current->wordid!=wordid)
		current=current->alter;
	if(current->wordid==wordid)
		return current;
	else
	{
		current->alter=(TERM *) malloc(sizeof(TERM));
		bytecount+=sizeof(TERM);
		current=current->alter;
		current->wordid=wordid;
		current->next=NULL;
		current->alter=NULL;
		current->isfinal=0;
		return current;
	}
};

TERM* Find(TERM *current, unsigned long int wordid)
{
	
	if(current->next==NULL) 
		return NULL;
	current=current->next;
	while(current->alter!=NULL && current->wordid!=wordid)
		current=current->alter;
	if(current->wordid==wordid)
		return current;
	else
		return NULL;
};

unsigned long int DictFind(unsigned char *word, NODE *dict)
{
	unsigned char *pt=word;
	NODE *node=dict;
	while((*pt!='\n') && (*pt!='\0') && (node->next[*pt]!=NULL))
		node=node->next[*pt++];
	if(((*pt=='\n') || (*pt=='\0')) && (node->number>0))
		return node->number;
	else
		return 0;	
}

void Blank(NODE *node)
{
	int i;
	if(node!=NULL)
		for(i=0; i<=255; i++)
			node->next[i]=NULL;
	node->number=0;	
}

void main(int argc, char *argv[])
{
	NODE dict, *cnode=&dict; /* dictionary (words) */
	TERM *thesaurus;	/* array of (multi-word) terms */
	TERM  *tnode;
	unsigned char cchar;
	unsigned long int nbwords=0, nbterms=0;
	int i;
	FILE *DictFile;
	char word[LINEMAXLENGTH];
	char input[LINEMAXLENGTH], *current;

	/* Initialisation */

	if(argc !=2)
	{
		fprintf(stderr, "Usage : %s Dictionary_file Document_file\n", argv[0]);
		exit(-1);
	}
	if((DictFile=fopen(argv[1], "r"))==NULL)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		exit(-1);
	}	
	Blank(cnode);
	
	/* Load dictionary in RAM */
	while(!feof(DictFile))
	{
		cchar=fgetc(DictFile);
		if(cchar=='\n'||cchar==' ')
		{
			if(nbwords%500000==0)
				printf("Dictionary loaded with %ld words (%ld Mb)\n", nbwords, bytecount/1024/1024);

			if(cnode->number==0) // Word is unseen ?
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
	
	/* Thesaurus initialisation */
	thesaurus= (TERM *) malloc(MAXWORDS*sizeof(TERM));
	bytecount+=MAXWORDS*sizeof(TERM);
	printf("Dictionary loaded with %ld words (%ld Mb)\n", nbwords, bytecount/1024/1024);
	for(nbwords=0; nbwords<MAXWORDS; nbwords++)
	{
		thesaurus[nbwords].next=NULL;
		thesaurus[nbwords].alter=NULL;
		thesaurus[nbwords].wordid=0;
		thesaurus[nbwords].isfinal=TRUE;
	}	
	if((DictFile=fopen(argv[1], "r"))==NULL)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		exit(-1);
	}
	/* Load Thesaurus */
	nbterms=0;
	while(!feof(DictFile))
	{
		if(fgets(input, LINEMAXLENGTH, DictFile)==NULL)
			break;
		nbterms++;
		current=input;
		sscanf(current,"%s", word);
		current+= strlen(word)+1;
		nbwords=DictFind(word, &dict); /* identify current word */
		thesaurus[nbwords].wordid=nbwords;
		tnode=&thesaurus[nbwords];
		while(sscanf(current,"%s", word)!=EOF)
		{
			tnode=FindOrCreateNextAlternative(tnode, DictFind(word, &dict));
			current+= strlen(word)+1;
		}
		tnode->isfinal=TRUE;
		if(nbterms%1000000==0)
			printf("Thesaurus loaded with %ld terms (%ld Mb)\n", nbterms, bytecount/1024/1024);
	};
	fclose(DictFile);
	printf("Thesaurus loaded with %ld terms (%ld Mb)\n", nbterms, bytecount/1024/1024);
		
	/* TEST */
	printf("Enter a term followed by <enter> :\n");
	while(!feof(stdin))
	{
		if(fgets(input, LINEMAXLENGTH, stdin)==NULL)
			break;
		current=input;
		sscanf(current,"%s", word);
		current+= strlen(word)+1;
		nbwords=DictFind(word, &dict); /* identify current word */
		if(thesaurus[nbwords].wordid==0)
		{
			printf("Search fail at %s\n", word);
		}
		else
		{
			tnode=&thesaurus[nbwords];
			while(sscanf(current,"%s", word)!=EOF)
			{
				tnode=Find(tnode, DictFind(word, &dict));
				if(tnode==NULL)
					break;
				current+= strlen(word)+1;
			}
			if(tnode!=NULL && tnode->isfinal)
				printf("The term was found\n");
			else
				printf("Search failed at %s\n", word);
		}
	}
}
		
