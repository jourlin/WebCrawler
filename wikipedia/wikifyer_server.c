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
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXWORDS 	2000000
#define LINEMAXLENGTH	500
#define BUFFERMAXLENGTH	(1024*1024)

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

	
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

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
	/* Thesaurus variables */
	NODE  *cnode=&dict; /* dictionary (words) */
	TERM  *tnode;
	unsigned char cchar;
	unsigned long int nbwords=0, nbterms=0;
	int i;
	FILE *DictFile;
	unsigned char word[LINEMAXLENGTH];
	unsigned char input[LINEMAXLENGTH], *current;

	/* Server variables */
	int sockfd, newsockfd, portno;
     	socklen_t clilen;
     	char buffer[BUFFERMAXLENGTH];
     	struct sockaddr_in serv_addr, cli_addr;
     	int n;

	/* Initialisation */
	#ifdef DEBUG
		printf("WARNING : %s was compiled with DEBUG and will process a maximum of 1000 terms\n", argv[0]);
	#endif
	if(argc !=3)
	{
		fprintf(stderr, "Usage : %s dictionary_file port\n", argv[0]);
		exit(-1);
	}
	if((DictFile=fopen(argv[1], "r"))==NULL)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		exit(-1);
	}	
	Blank(cnode);
	nbterms=0;
	/* Load dictionary in RAM */
	while(!feof(DictFile))
	{
		#ifdef DEBUG
		nbterms++;
		if(nbterms>1000) 
			break;
		#endif

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
#ifdef DEBUG
		if(nbterms>1000) 
			break;
#endif
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
		
	/* Server code */
	printf("Waiting for requests...\n");
	unsigned char *eterm, term[LINEMAXLENGTH];	/* pointers to start and end of term */
	while(1)					/* TODO : process signals */
	{	
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
     		if (sockfd < 0) 
    	    	error("ERROR opening socket");
   	  	bzero((char *) &serv_addr, sizeof(serv_addr));
    	 	portno = atoi(argv[2]);
    	 	serv_addr.sin_family = AF_INET;
   	  	serv_addr.sin_addr.s_addr = INADDR_ANY;
   	  	serv_addr.sin_port = htons(portno);
   	  	if (bind(sockfd, (struct sockaddr *) &serv_addr,
   	           sizeof(serv_addr)) < 0) 
   	           error("ERROR on binding");	
		listen(sockfd,5);
     		clilen = sizeof(cli_addr);
     		newsockfd = accept(sockfd, 
     	          	 (struct sockaddr *) &cli_addr, 
     	         	   &clilen);
    	 	if (newsockfd < 0) 
    		      	error("ERROR on accept");
    		while(1) 				/* For each line sent by a client */			
		{				     
			bzero(buffer,BUFFERMAXLENGTH);
     			n = read(newsockfd,buffer,BUFFERMAXLENGTH);
     			if (n <= 0)  
				break;			/* end of connection */
     			else	
				printf("Received: %s\n",buffer);
			current=buffer;
			input[0]='\0';
			while(sscanf(current,"%s", word)!=EOF) /* For each possible term beginning */
			{
				eterm=FindLonguestTerm(current);
				if(eterm==NULL)
				{	/* Next word */
					current+= strlen(word)+1;
				}
				else
				{	/* A term was found */
					strncpy(term+2, current, eterm-current);
					term[eterm-current+1]='\0';
					term[0]=term[1]='[';
					while(term[strlen(term)-1]==' '||term[strlen(term)-1]=='\n') /* remove trailing blanks */
						term[strlen(term)-1]='\0';
					term[strlen(term)+2]='\0';
					term[strlen(term)]=term[strlen(term)+1]=']';
					printf("Sending %s\n", term);			
					strcat(input, term);
					strcat(input,"\n");	
					current=eterm;
				}
			}
			n = write(newsockfd,input,strlen(input));
	     		if (n < 0) 
				error("ERROR writing to socket");
		
		}
		close(newsockfd);
		close(sockfd);
	}	
     	return; 
}
		
