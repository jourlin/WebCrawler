/* Processed by ecpg (4.5.0) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "ecpg/Anelosimus.Eximius.pgc"
/* 
  Anelosimus.Eximius.pgc : This process gets a list of URLs from 
  the table "node", downloads them, inserts the new URLs into table node,
  and new links into table "links".

    Copyright (C) 2011  Pierre Jourlin

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
  Anelosimus.Eximius.pgc : Ce processus récupère une liste d'URLs dans
  la table "node", les télécharge, insère dans la même table les nouvelles
  URLs découvertes et dans la table "links", les nouveaux liens.

  Copyright (C) 2011 Pierre Jourlin — Tous droits réservés.
 
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

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <curl/multi.h>
#include <sys/types.h> 
#include <signal.h>

#define MAXURLSIZE		1023 /* should be less than max chars for type url */
#define MAXCO			21	// Max parrallel connections
#define MAXSMALLSTRING		512	// Max number of chars in top level domain
#define MAXCONTEXT		100	// Max chars for hyperlinks context


#line 1 "/usr/include/postgresql/sqlca.h"
#ifndef POSTGRES_SQLCA_H
#define POSTGRES_SQLCA_H

#ifndef PGDLLIMPORT
#if  defined(WIN32) || defined(__CYGWIN__)
#define PGDLLIMPORT __declspec (dllimport)
#else
#define PGDLLIMPORT
#endif   /* __CYGWIN__ */
#endif   /* PGDLLIMPORT */

#define SQLERRMC_LEN	150

#ifdef __cplusplus
extern		"C"
{
#endif

struct sqlca_t
{
	char		sqlcaid[8];
	long		sqlabc;
	long		sqlcode;
	struct
	{
		int			sqlerrml;
		char		sqlerrmc[SQLERRMC_LEN];
	}			sqlerrm;
	char		sqlerrp[8];
	long		sqlerrd[6];
	/* Element 0: empty						*/
	/* 1: OID of processed tuple if applicable			*/
	/* 2: number of rows processed				*/
	/* after an INSERT, UPDATE or				*/
	/* DELETE statement					*/
	/* 3: empty						*/
	/* 4: empty						*/
	/* 5: empty						*/
	char		sqlwarn[8];
	/* Element 0: set to 'W' if at least one other is 'W'	*/
	/* 1: if 'W' at least one character string		*/
	/* value was truncated when it was			*/
	/* stored into a host variable.				*/

	/*
	 * 2: if 'W' a (hopefully) non-fatal notice occurred
	 */	/* 3: empty */
	/* 4: empty						*/
	/* 5: empty						*/
	/* 6: empty						*/
	/* 7: empty						*/

	char		sqlstate[5];
};

struct sqlca_t *ECPGget_sqlca(void);

#ifndef POSTGRES_ECPG_INTERNAL
#define sqlca (*ECPGget_sqlca())
#endif

#ifdef __cplusplus
}
#endif

#endif

#line 67 "ecpg/Anelosimus.Eximius.pgc"

/* exec sql begin declare section */
	  
		 
		   
	
struct MemoryStruct { 
#line 70 "ecpg/Anelosimus.Eximius.pgc"
 char * memory ;
 
#line 71 "ecpg/Anelosimus.Eximius.pgc"
 unsigned long size ;
 } ;/* exec sql end declare section */
#line 73 "ecpg/Anelosimus.Eximius.pgc"

/* exec sql begin declare section */
	   
	 
	  
	 
	 
	   
	 
	 
	 
	 
	 
	 
	 
	 
	   

#line 75 "ecpg/Anelosimus.Eximius.pgc"
 unsigned long MaxPageSize = 0 ;
 
#line 76 "ecpg/Anelosimus.Eximius.pgc"
 int nurls ;
 
#line 77 "ecpg/Anelosimus.Eximius.pgc"
 struct MemoryStruct currentBody [ MAXCO ] ;
 
#line 78 "ecpg/Anelosimus.Eximius.pgc"
 char currentURL [ MAXURLSIZE ] ;
 
#line 79 "ecpg/Anelosimus.Eximius.pgc"
 char url_chunk [ MAXCO ] [ MAXURLSIZE ] ;
 
#line 80 "ecpg/Anelosimus.Eximius.pgc"
 unsigned long url_id [ MAXCO ] ;
 
#line 81 "ecpg/Anelosimus.Eximius.pgc"
 char url_enc [ MAXCO ] [ MAXSMALLSTRING ] ;
 
#line 82 "ecpg/Anelosimus.Eximius.pgc"
 char LinkLeftContext [ MAXURLSIZE ] ;
 
#line 83 "ecpg/Anelosimus.Eximius.pgc"
 char LinkMidContext [ MAXURLSIZE ] ;
 
#line 84 "ecpg/Anelosimus.Eximius.pgc"
 char LinkRightContext [ MAXURLSIZE ] ;
 
#line 85 "ecpg/Anelosimus.Eximius.pgc"
 char currentRoot [ MAXURLSIZE ] ;
 
#line 86 "ecpg/Anelosimus.Eximius.pgc"
 char currentRootURL [ MAXURLSIZE ] ;
 
#line 87 "ecpg/Anelosimus.Eximius.pgc"
 char currentEffectiveURL [ MAXURLSIZE ] ;
 
#line 88 "ecpg/Anelosimus.Eximius.pgc"
 char currentEncoding [ MAXSMALLSTRING ] ;
 
#line 89 "ecpg/Anelosimus.Eximius.pgc"
 unsigned long currentID ;
/* exec sql end declare section */
#line 90 "ecpg/Anelosimus.Eximius.pgc"

/* exec sql begin declare section */
     // Data for connection if needed
    
   


#line 92 "ecpg/Anelosimus.Eximius.pgc"
 const char * target = _TARGET_ ;
 
#line 93 "ecpg/Anelosimus.Eximius.pgc"
 const char * user = _USER_ ;
 
#line 94 "ecpg/Anelosimus.Eximius.pgc"
 const char * password = _PASSWORD_ ;
/* exec sql end declare section */
#line 96 "ecpg/Anelosimus.Eximius.pgc"


void terminate(int sig);

void checkErrorCode(void){
	if(sqlca.sqlcode!=0){
		if(sqlca.sqlcode!=-403){
    			fprintf(stderr,"error code %ld, message [%s], rows %ld, warning %c\n", 
    			sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrd[2], 
    			sqlca.sqlwarn[0]);
			if(currentRoot!=NULL)
				fprintf(stderr, "While Processing %s with enc=%s\n", currentRoot, currentEncoding);
		}
		else {
			// not necessarily wrong : might already got this info
		}
	}
}


size_t curl_write( void *ptr, size_t size, size_t nmemb, void *userdata)
{	
	struct MemoryStruct *mem= &currentBody[(long int) userdata];
	mem->memory=realloc(mem->memory, ((size_t) mem->size+1) + size*nmemb); // extend memory
	
	if(mem->memory==NULL){
   		 /* out of memory! */ 
    		printf("not enough memory (realloc returned NULL)\n");
    		exit(EXIT_FAILURE);
  	}
	memcpy(&(mem->memory[mem->size]), ptr, size*nmemb);
	mem->size+=(unsigned long int) (size*nmemb);
	mem->memory[mem->size] = 0;
	return size*nmemb;
}


void drop_blanks(char *mem){
	char *pt=mem;
	while(*pt!=0){				// parse until the end of string
		while(*pt!=0 && *pt!=' ' && *pt!='\t' && *pt !='\r' && *pt!='\n')	// copy text
			*mem++=*pt++;
		while(*pt!=0 && (*pt==' '||*pt=='\t'||*pt=='\r'||*pt=='\n'))	// ignore blanks
			pt++;
		if(*pt)
			*mem++=' ';
	};
	*mem=0;					// terminate the new, shortened string
}


int getNextURL(bool reinit, int idx){
	static const char *tag="href=\"";
	static char *start;
	static int minlength=65535, maxlength=-1;
	*LinkLeftContext='\0';
	*LinkMidContext='\0';
	*LinkRightContext='\0';

	if(reinit){
		start=currentBody[idx].memory;
		return TRUE;
	}
	char *end=start;
	char *startleft, *endleft;
	char *startmid, *endmid;
	char *startright, *endright;

	start=strstr(start, tag);
	
	if(start!=NULL){
		start+=strlen(tag);
		end=strstr(start, "\"");
		if(end!=NULL){
			if((end-start) <MAXURLSIZE){
				memcpy(currentURL, start, (size_t) (end-start));
				*(currentURL+(size_t) (end-start))='\0';
				
				/*********************/
				/* Find Left Context */
				/*********************/
				
				startleft=start; // On href=
				while((startleft >=currentBody[idx].memory) &&  (*startleft!='<'))
					startleft--; // Find the origin of the <A tag
				endleft=startleft;   // End of left context
				while((startleft >=currentBody[idx].memory) && ((endleft-startleft) < MAXCONTEXT) 
					&& (*startleft!='>') && (*startleft!='\n'))
					startleft--; // Find the origin of the <A tag
				if(endleft-startleft>1){
					memcpy(LinkLeftContext, startleft+1, (size_t) (endleft-startleft)-1);
					*(LinkLeftContext+(size_t) (endleft-startleft)-1)='\0';
					drop_blanks(LinkLeftContext);
					};
				
				/***********************/
				/* Find Middle Context */
				/***********************/
				
				startmid=strstr(start, ">");
				if((startmid!=NULL) && (*(startmid-1)!='/'))
					endmid=strstr(startmid, "<");
				else
					endmid=startmid+1;
				if(startmid==NULL||endmid==NULL||(endmid-startmid)>=MAXURLSIZE||endmid==(startmid+1))
					*LinkMidContext='\0';
				else{
					memcpy(LinkMidContext, startmid+1, (size_t) (endmid-startmid)-1);
					*(LinkMidContext+(size_t) (endmid-startmid)-1)='\0';
					drop_blanks(LinkMidContext);
					//printf("Mid : {%s}\n",LinkMidContext); 
				}
				
				/***********************/
				/* Find Right Context  */
				/***********************/
				
				if(endmid!=NULL){
					startright=strstr(endmid, ">");
					if(startright!=NULL){ 
						startright++;
						endright=strstr(startright-1, "<");
					}
				}
				else
					startright=NULL;
				if(startright==NULL || endright==NULL || (endright-startright <1))
					*LinkRightContext='\0';
				else{
					if(endright-startright<minlength){
						minlength=endright-startright;
						printf("New min right length : [%d]\n",minlength);
					}
					if(endright-startright>maxlength){
						maxlength=endright-startright;
						printf("New max right length : [%d]\n",maxlength);
					}
					if(endright-startright>=MAXCONTEXT) // trim
						endright=startright+MAXCONTEXT -1;
					if(endright-startright>0)
						memcpy(LinkRightContext, startright, (size_t) (endright-startright));
					*(LinkRightContext+(size_t) (endright-startright))='\0';
					drop_blanks(LinkRightContext);
				}
				
			}
			else
				end=NULL;
		}
		start=end;
	}
	return (start!=NULL);
}

static void init(CURLM *cm, int i, unsigned long header)
{
  CURL *eh = curl_easy_init();
  
  curl_easy_setopt(eh, CURLOPT_URL, url_chunk[i]);
  curl_easy_setopt(eh, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(eh, CURLOPT_MAXREDIRS, -1);
  curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, curl_write);
  curl_easy_setopt(eh, CURLOPT_WRITEDATA, (void *) (long int) i);
  curl_easy_setopt(eh, CURLOPT_HEADER, header);
  curl_easy_setopt(eh, CURLOPT_NOBODY, header);
  curl_easy_setopt(eh, CURLOPT_TIMEOUT, 1L); 
  curl_easy_setopt(eh, CURLOPT_PRIVATE, i);
  curl_easy_setopt(eh, CURLOPT_VERBOSE, 0L);
  curl_easy_setopt(eh, CURLOPT_DNS_CACHE_TIMEOUT, 48*60*60); // 48h 
  curl_multi_add_handle(cm, eh);
}



void terminate(int sig) {
	{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update node set checked = null , effectiveurl = null where url = $1 ", 
	ECPGt_char,(currentRootURL),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 271 "ecpg/Anelosimus.Eximius.pgc"

	{ ECPGdisconnect(__LINE__, "ALL");}
#line 272 "ecpg/Anelosimus.Eximius.pgc"

        printf("\nInterrupted ! Cancelling the crawl on %s\n", currentRootURL);
	curl_global_cleanup();
        exit(sig);
}

void xhtml2text(char *mem){
	char *pt=mem;
	while(*pt!=0){				// parse until the end of string
		while(*pt!=0 && *pt!='<')	// copy text when outside a tag
			*mem++=*pt++;
		while(*pt!=0 && *pt!='>')	// ignore text when inside a tag
			pt++;
		if(*pt){
			*mem++='\n';
			pt++;
		}
	};
	*mem=0;					// terminate the new, shortened string
}

void drop_content(char *mem, const char *tstart, const char *tend){
	char *start, *end;

	start=strstr(mem, tstart);
	do{
		if(start==NULL)	// Done
			return;
		else
			end=strstr(start, tend);
		if(end==NULL)
			*start=0;	// remove all the text starting with <script 
		else
			strcpy(start, end+strlen(tend));	// remove the text between script tags
		start=strstr(start, tstart);
	}while(1);
}

int main(void) {
	CURL *curl;
	CURLcode res;
	char *tmp;

	CURLM *cm;
  	CURLMsg *msg;
  	long nburls, L;
  	unsigned int C=0;
  	int M, Q, U;
  	fd_set R, W, E;
  	struct timeval T;

	pid_t pid;
	if ((pid = getpid()) < 0) {
	  perror("unable to get pid");
	};
	

	(void) signal(SIGINT,terminate);

// 	uncomment the following line if you need database details for the connection
// 	if no details are needed, simply do :
//	exec sql connect to 'unix:postgresql:locahost' ;
//	exec sql SET CLIENT_ENCODING TO 'UTF8'; 
	while(1){
		{ ECPGconnect(__LINE__, 0, target , user , password , NULL, 1); }
#line 336 "ecpg/Anelosimus.Eximius.pgc"

		checkErrorCode();
		M=Q=U=-1; // re-init error flags
		{ ECPGtrans(__LINE__, NULL, "begin work");}
#line 339 "ecpg/Anelosimus.Eximius.pgc"

		{ ECPGprepare(__LINE__, NULL, 0, "get_url", "SELECT url_out(url) as url, id FROM node WHERE checked IS NULL AND depth<4 AND score IS NOT NULL ORDER BY score DESC, url_len(url) ASC LIMIT 20 FOR UPDATE ;");}
#line 340 "ecpg/Anelosimus.Eximius.pgc"

		checkErrorCode();
		{ ECPGtrans(__LINE__, NULL, "commit work");}
#line 342 "ecpg/Anelosimus.Eximius.pgc"

		{ ECPGtrans(__LINE__, NULL, "begin work");}
#line 343 "ecpg/Anelosimus.Eximius.pgc"

		/* declare url_cursor cursor for $1 */
#line 344 "ecpg/Anelosimus.Eximius.pgc"

		checkErrorCode();
		/* when end of result set reached, break out of while loop */
		/* exec sql whenever not found  break ; */
#line 347 "ecpg/Anelosimus.Eximius.pgc"

		checkErrorCode();
		{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "declare url_cursor cursor for $1", 
	ECPGt_char_variable,(ECPGprepared_statement(NULL, "get_url", __LINE__)),(long)1,(long)1,(1)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 349 "ecpg/Anelosimus.Eximius.pgc"


		checkErrorCode();
		nburls=0;
		while(1){
			{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch next from url_cursor", ECPGt_EOIT, 
	ECPGt_char,(currentRootURL),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_unsigned_long,&(currentID),(long)1,(long)1,sizeof(unsigned long), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 354 "ecpg/Anelosimus.Eximius.pgc"

if (sqlca.sqlcode == ECPG_NOT_FOUND) break;}
#line 354 "ecpg/Anelosimus.Eximius.pgc"

			//printf("[%d] is processing %s\n", pid, currentRootURL );
			checkErrorCode();
			if(sqlca.sqlcode!=0){
				break;
			}
			{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update node set checked = now ( ) where id = $1 ", 
	ECPGt_unsigned_long,&(currentID),(long)1,(long)1,sizeof(unsigned long), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);
#line 360 "ecpg/Anelosimus.Eximius.pgc"

if (sqlca.sqlcode == ECPG_NOT_FOUND) break;}
#line 360 "ecpg/Anelosimus.Eximius.pgc"

	                checkErrorCode();
	                if(sqlca.sqlcode!=0){
	                        break;
			}
			strcpy(url_chunk[nburls], currentRootURL);
			url_id[nburls]=currentID;
			nburls++;
		}
		{ ECPGtrans(__LINE__, NULL, "commit work");}
#line 369 "ecpg/Anelosimus.Eximius.pgc"

		curl_global_init(CURL_GLOBAL_ALL);
		cm = curl_multi_init();
		for (C = 0; C < nburls; ++C) {
    			init(cm, C, 1);			/* Only headers at the moment */
			getNextURL(TRUE, C); 		// initialize
			currentBody[C].memory=malloc(1); // For the null char
			currentBody[C].size=0;	// no char at this point  

  		}
		nburls=0; // reused for counting html and xml urls
		while (U) {
			curl_multi_perform(cm, &U);
	 		if (U) {
 				FD_ZERO(&R);
				FD_ZERO(&W);
				FD_ZERO(&E);
 
				if (curl_multi_fdset(cm, &R, &W, &E, &M)) {
					fprintf(stderr, "E: curl_multi_fdset\n");
					return EXIT_FAILURE;
				}
 
				if (curl_multi_timeout(cm, &L)) {
					fprintf(stderr, "E: curl_multi_timeout\n");
					return EXIT_FAILURE;
				}
				if (L == -1)
					L = 100;
	 
				if (M == -1) {
					#ifdef WIN32
					Sleep(L);
					#else
					sleep(L / 1000);
					#endif
				} else {
					T.tv_sec = L/1000;
					T.tv_usec = (L%1000)*1000;
 					if (0 > select(M+1, &R, &W, &E, &T)) {
						fprintf(stderr, "E: select(%i,,,,%li): %i: %s\n", M+1, L, errno, strerror(errno));
        	  				return EXIT_FAILURE;
        				}
      				}
			}
 			while ((msg = curl_multi_info_read(cm, &Q))) {
				if (msg->msg == CURLMSG_DONE) {
					char *url, *eurl, *ct;
					CURL *e = msg->easy_handle;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &nburls);
					curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &eurl);
					curl_easy_getinfo(msg->easy_handle, CURLINFO_CONTENT_TYPE, &ct);
					strcpy(url_enc[nburls],"LATIN1"); // Default Charset

					if( (ct!=NULL) && 
							(	(strstr(ct, "text/html")!=NULL) 
								|| (strstr(ct, "text/xml")!=NULL)
							)
						){
						// fprintf(stderr,"CT:[%s]\n", ct);
						if(nburls<0 || nburls>=MAXCO){
							printf("Ooops ! : nburls=%ld\nBye...\n", nburls);
							exit(-1);
						}
						strcpy(currentEffectiveURL, eurl);
						strcpy(currentURL, url_chunk[nburls]);
						strcpy(url_chunk[nburls], eurl);
						if(strstr(ct, "utf-8")!=NULL || strstr(ct, "UTF-8")!=NULL)
							strcpy(url_enc[nburls],"UTF8");
						//printf("Warning : %ld[%s] for %s\n", nburls, url_enc[nburls], url_chunk[nburls]);
                                                { ECPGtrans(__LINE__, NULL, "begin work");}
#line 439 "ecpg/Anelosimus.Eximius.pgc"

						{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update node set effectiveurl = $1  where url = $2 ", 
	ECPGt_char,(currentEffectiveURL),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(currentURL),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);
#line 440 "ecpg/Anelosimus.Eximius.pgc"

if (sqlca.sqlcode == ECPG_NOT_FOUND) break;}
#line 440 "ecpg/Anelosimus.Eximius.pgc"

						checkErrorCode();
						{ ECPGtrans(__LINE__, NULL, "end work");}
#line 442 "ecpg/Anelosimus.Eximius.pgc"

					}
					curl_multi_remove_handle(cm, e);
					curl_easy_cleanup(e);
				}
				else {
        				fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
				}
				nburls++;
			}
		}
		for (C = 0; C < nburls; ++C) 
			if(currentBody[C].memory){
				free(currentBody[C].memory);
			}

		/***   Get links ****/
		M=Q=U=-1; // re-init error flags
		for (C = 0; C < nburls; ++C) {
    			init(cm, C, 0);			/* Now get Full pages */
			getNextURL(TRUE, C); // initialize
			currentBody[C].memory=malloc(1); // for the null char
			currentBody[C].size=0;	// no data at this point, only the terminal null char  
  		}

		while (U) {
			curl_multi_perform(cm, &U);
	 		if (U) {
 				FD_ZERO(&R);
				FD_ZERO(&W);
				FD_ZERO(&E);
 
				if (curl_multi_fdset(cm, &R, &W, &E, &M)) {
					fprintf(stderr, "E: curl_multi_fdset\n");
					return EXIT_FAILURE;
				}
 
				if (curl_multi_timeout(cm, &L)) {
					fprintf(stderr, "E: curl_multi_timeout\n");
					return EXIT_FAILURE;
				}
				if (L == -1)
					L = 100;
	 
				if (M == -1) {
					#ifdef WIN32
					Sleep(L);
					#else
					sleep(L / 1000);
					#endif
				} else {
					T.tv_sec = L/1000;
					T.tv_usec = (L%1000)*1000;
 					if (0 > select(M+1, &R, &W, &E, &T)) {
						fprintf(stderr, "E: select(%i,,,,%li): %i: %s\n", M+1, L, errno, strerror(errno));
        	  				return EXIT_FAILURE;
        				}
      				}
			}

 			while ((msg = curl_multi_info_read(cm, &Q))) {
				if (msg->msg == CURLMSG_DONE) {
					char *url;
					CURL *e = msg->easy_handle;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &url);
					//fprintf(stderr, "R: %d - %s <%s>\n", msg->data.result, curl_easy_strerror(msg->data.result), url);
					curl_multi_remove_handle(cm, e);
					curl_easy_cleanup(e);
				}
				else {
        				fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
				}
			}
		}
		for (C = 0; C < nburls; ++C) {
			getNextURL(TRUE, C);
			strcpy(currentURL, url_chunk[C]);
			strcpy(currentRootURL, currentURL); 
			tmp=strstr(currentRootURL,"://"); /* Extract root website URL */
			if(tmp!=NULL)
				tmp=strstr(tmp+3, "/");
			if(tmp!=NULL)
				*tmp='\0';
			strcpy(currentEncoding, url_enc[C]);
			currentID=url_id[C];
			fprintf(stderr,"process %d is processing [%s], %d, encoding [%s]\n", pid, currentURL, C, url_enc[C]);
			
  	 		while(getNextURL(FALSE, C)){
				{ ECPGtrans(__LINE__, NULL, "begin work");}
#line 530 "ecpg/Anelosimus.Eximius.pgc"

				{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set CLIENT_ENCODING to $0", 
	ECPGt_char,(currentEncoding),(long)MAXSMALLSTRING,(long)1,(MAXSMALLSTRING)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 531 "ecpg/Anelosimus.Eximius.pgc"
 
				if(strlen(currentURL)>1 && (currentURL[0]=='/'||currentURL[0]=='.'||currentURL[0]=='.')){

					strcpy(currentRoot, currentRootURL);
					if(currentURL[0]!='/' || currentRoot[strlen(currentRoot)-1]!='/')
						strcat(currentRoot, currentURL);
					else 
						strcat(currentRoot, currentURL+1);
				}
				else
					strcpy(currentRoot, currentURL);
				if(strlen(currentRoot)>7 && (!strncmp(currentRoot, "http://",7) || !strncmp(currentRoot, "https://",8) || !strncmp(currentRoot, "ftp://",6) || !strncmp(currentRoot, "ftps://",7))){
					{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "insert into linksview ( \"from\" , \"to\" , \"leftcontext\" , \"midcontext\" , \"rightcontext\" ) values ( $1  , $2  , $3  , $4  , $5  )", 
	ECPGt_unsigned_long,&(currentID),(long)1,(long)1,sizeof(unsigned long), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(currentRoot),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(LinkLeftContext),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(LinkMidContext),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(LinkRightContext),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);
#line 543 "ecpg/Anelosimus.Eximius.pgc"

if (sqlca.sqlcode == ECPG_NOT_FOUND) break;}
#line 543 "ecpg/Anelosimus.Eximius.pgc"
			
					checkErrorCode();
				}
				{ ECPGtrans(__LINE__, NULL, "end work");}
#line 546 "ecpg/Anelosimus.Eximius.pgc"

					
			}
			//printf("Done.\n");
			if(sqlca.sqlcode==0){
				if(currentBody[C].size > MaxPageSize){
					MaxPageSize=currentBody[C].size;
					// fprintf(stderr, "*** New maximum page size %ld\n", MaxPageSize);
				}
				drop_content(currentBody[C].memory, "<!--", "-->");
				drop_content(currentBody[C].memory, "<script ", "</script>");
				drop_content(currentBody[C].memory, "<style ", "</style>");
				xhtml2text(currentBody[C].memory);
				drop_blanks(currentBody[C].memory);
				{ ECPGtrans(__LINE__, NULL, "begin work");}
#line 560 "ecpg/Anelosimus.Eximius.pgc"

				{ ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update node set content = $1  where url = $2 ", 
	ECPGt_char,&(currentBody[C].memory),(long)0,(long)1,(1)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, 
	ECPGt_char,(url_chunk[C]),(long)MAXURLSIZE,(long)1,(MAXURLSIZE)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);
#line 561 "ecpg/Anelosimus.Eximius.pgc"

if (sqlca.sqlcode == ECPG_NOT_FOUND) break;}
#line 561 "ecpg/Anelosimus.Eximius.pgc"

				checkErrorCode();
				{ ECPGtrans(__LINE__, NULL, "end work");}
#line 563 "ecpg/Anelosimus.Eximius.pgc"

			}
			//printf("Content of %s:\n [%s]\n", url_chunk[C], currentBody[C].memory);
			if(currentBody[C].memory){
				//printf("Free2 :%d\n", C);
				free(currentBody[C].memory);
			}
  		}
		curl_multi_cleanup(cm);
		curl_global_cleanup();
		{ ECPGdisconnect(__LINE__, "ALL");}
#line 573 "ecpg/Anelosimus.Eximius.pgc"
	
	}
	return EXIT_SUCCESS;
}

