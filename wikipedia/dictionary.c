/* 
  dictionary.c : Extracts wikipedia article's titles from a UTF-8 text.

    Copyright (C) 2012  Pierre Jourlin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************************
  dictionary.c : Extrait les titres d'articles de wikipedia qui apparaissent dans un texte en UTF8.

  Copyright (C) 2012 Pierre Jourlin — Tous droits réservés.
 
  Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
  modifier suivant les termes de la “GNU General Public License” telle que
  publiée par la Free Software Foundation : soit la version 3 de cette
  licence, soit (à votre gré) toute version ultérieure.
  
  Ce programme est distribué dans l’espoir qu’il vous sera utile, mais SANS
  AUCUNE GARANTIE : sans même la garantie implicite de COMMERCIALISABILITÉ
  ni d’ADÉQUATION À UN OBJECTIF PARTICULIER. Consultez la Licence Générale
  Publique GNU pour plus de détails.
  
  Vous devriez avoir reçu une copie de la Licence Générale Publique GNU avec
  ce programme ; si ce n’est pas le cas, consultez :
  <http://www.gnu.org/licenses/>.

    Pierre Jourlin
    L.I.A. / C.E.R.I.
    339, chemin des Meinajariès
    BP 1228 Agroparc
    84911 AVIGNON CEDEX 9
    France 
    pierre.jourlin@univ-avignon.fr
    Tel : +33 4 90 84 35 32
    Fax : +33 4 90 84 35 01

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORDS 	2000000
#define LINEMAXLENGTH	500

#define TRUE 		1
#define FALSE		0

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
NODE dict;
TERM *thesaurus;	/* array of (multi-word) terms */

	

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
unsigned char* FindLonguestTerm(char * start)
{
	unsigned char word[LINEMAXLENGTH], *current=start, *longuest=NULL;
	unsigned long int wordid;
	TERM *tnode;
	if(sscanf(current,"%s", word)==EOF)
		return NULL;
	wordid=DictFind(word, &dict); /* identify current word */
	if(thesaurus[wordid].wordid==0) /* word was not recognized */
		return NULL;
	tnode=&thesaurus[wordid];
	current+=strlen(word)+1;
	if(tnode->isfinal)
		longuest=current;
	while(sscanf(current,"%s", word)!=EOF)
	{
		tnode=Find(tnode, DictFind(word, &dict));
		if(tnode==NULL)
			break;
		current+= strlen(word)+1;
		if(tnode->isfinal)
			longuest=current;
	}
	return longuest;
}

void main(int argc, char *argv[])
{
	NODE  *cnode=&dict; /* dictionary (words) */
	TERM  *tnode;
	unsigned char cchar;
	unsigned long int nbwords=0, nbterms=0;
	int i;
	FILE *DictFile;
	unsigned char word[LINEMAXLENGTH];
	unsigned char input[LINEMAXLENGTH], *current;

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
	unsigned char *eterm, term[LINEMAXLENGTH];  /* pointers to start and end of term */
	while(!feof(stdin))
	{
		if(fgets(input, LINEMAXLENGTH, stdin)==NULL)
			break;
		current=input;
		while(sscanf(current,"%s", word)!=EOF)
		{
			eterm=FindLonguestTerm(current);
			if(eterm==NULL)
			{	/* Next word */
				current+= strlen(word)+1;
			}
			else
			{	/* A term was found */
				strncpy(term, current, eterm-current);
				term[eterm-current]='\0';
				if(term[strlen(term)-1]==' '||term[strlen(term)-1]=='\n')
					term[strlen(term)-1]='\0';				
				printf("[%s]\n", term);
				current=eterm;
			}
		}
		
	}
}
		
