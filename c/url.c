/* 
  url.c : source code for the user-defined PostgreSQL data type "url".

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
  url.c : code source pour le type "url" (type de donnée PostgreSQL defini par l'utilisateur).

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

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */


PG_MODULE_MAGIC;

typedef struct Url
{
	char	str[255];
	unsigned short	length;
	unsigned short	subdomains;
	unsigned short	seconddomain;
	unsigned short	topdomain;
	unsigned short	path;
}	Url;

/*
 * Since we use V1 function calling convention, all these functions have
 * the same signature as far as C is concerned.  We provide these prototypes
 * just to forestall warnings when compiled with gcc -Wmissing-prototypes.
 */
Datum		url_in(PG_FUNCTION_ARGS);
Datum		url_out(PG_FUNCTION_ARGS);
Datum		url_top(PG_FUNCTION_ARGS);


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(url_in);

Datum
url_in(PG_FUNCTION_ARGS)
{
	char	   *strin = PG_GETARG_CSTRING(0);
	unsigned short	length;
	unsigned short	subdomains;
	unsigned short	seconddomain;
	unsigned short	topdomain;
	unsigned short	path;

	Url    *result;
	result = (Url *) palloc(sizeof(Url));
	strcpy(result->str, strin);			/* copy the URL */
	length=strlen(strin);
	result->length = length;			/* copy length */
	if(length>255)					/* Check URL length */
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("URL should contain less strictly less than 256 characters : \"%s\"",
						strin)));
	/* Search for the end of the protocol part */
	while(*strin!='\0' && *strin!='/')
		strin++;
	if(*strin=='\0' || *(++strin)!='/')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("URL should start with a protocol (e.g. http://) : \"%s\"",
						strin)));
	++strin;
	subdomains= (unsigned short) (strin - PG_GETARG_CSTRING(0)); /* character offset for the subdomain part of the url */
	/* Search for the end of the root URL */
	while(*strin!='\0' && *strin!='/')
		strin++;
	path= (unsigned short) (strin - PG_GETARG_CSTRING(0));
	while(--strin>=PG_GETARG_CSTRING(0) && *strin!='.') /* find the dot before the top level domain */
		/* do nothing */;
	if(*strin!='.')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("URL should contain a top-level domain (e.g. .com) : \"%s\"",
						result->str)));
	topdomain=(unsigned short) (strin - PG_GETARG_CSTRING(0)+1); 	/* character offset for the top-level domain */
	while(--strin>=PG_GETARG_CSTRING(0) && *strin!='/' && *strin!='.') /* find the dot before the second level domain */
		/* do nothing */;
	if(*strin!='.' && *strin!='/')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("URL should contain a second-level domain (e.g. mydomain[.com]) : \"%s\"",
						strin)));
	seconddomain= (unsigned short) (strin - PG_GETARG_CSTRING(0)+1); 	/* character offset for the top-level domain */
	
	result->subdomains = subdomains;
	result->seconddomain = seconddomain;
	result->topdomain=topdomain;
	result->path=path;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(url_out);

Datum
url_out(PG_FUNCTION_ARGS)
{
	Url    *url = (Url *) PG_GETARG_POINTER(0);
	char	   *result;

	result = (char *) palloc(url->length+1+5*5); /* 5 numbers of at most 5 digits each */
	snprintf(result, url->length+1+5*5, "%s[%u,%u,%u,%u,%u]", url->str, url->length, url->subdomains, url->seconddomain, url->topdomain, url->path);
	PG_RETURN_CSTRING(result);
}

Datum
url_top(PG_FUNCTION_ARGS)
{
	Url    *url = (Url *) PG_GETARG_POINTER(0);
	char	   *result;
	result = (char *) palloc(100);
	snprintf(result, 100, "%s", url->str);
	PG_RETURN_CSTRING(result);
}


