#include "cclxpop.h"
#include "cclxrecv.h"

#include <alloc.h>
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


void new_init(char *directive, char *value)
{

if (!strnicmp(directive, "cclxpop.", 8))
	{
	strcpy(directive, directive+6);
	}
if (!stricmp(directive, "timezone"))
	{
	if (*value)
		timezone = atol(value);
	else 	timezone = 0;
	}
if (!stricmp(directive, "sigfile"))
	{
	if (*value)
		sigfile = strdup(value);
	else 	sigfile = 0;
	}
if (!stricmp(directive, "from"))
	{
	if (*value)
		from  = strdup(value);
	else 	from  = 0;
	}
if (!stricmp(directive, "ccPath"))
	{
	if (*value)
		ccPath = strdup(value);
	else	ccPath = 0;
	}
if (!stricmp(directive, "spooldir"))
	{
	if (*value)
		spooldir = strdup(value);
	else	spooldir = 0;
	}
if (!stricmp(directive, "verbose"))
	{
	if (*value)
		verbose=atoi(value);
	else	verbose=0;
	}
if (!stricmp(directive, "append"))
	{
	if (*value)
		appendhdr=atoi(value);
	else	appendhdr=0;
	}
if (!stricmp(directive, "outboxfolder"))
	{
	if (*value)
		outboxfolder=atoi(value);
	else	outboxfolder=1;
	}
if (!stricmp(directive, "sentfolder"))
	{
	if (*value)
		sentfolder=atoi(value);
	else	sentfolder=2;
	}
}


int tcp_config( void )
{
    char name[80];
    char value[80], ch[2];
    int  quotemode;
    int f, mode;

if ( ( f = _open("cclxpop.cfg", O_RDONLY | O_TEXT )) == -1 )
	{
	printf("Cannot Find cclxpop.cfg. Aborting.\n");
	exit(-1);
	}
    *name = *value = mode = ch[1] = quotemode = 0;
    while ( _read( f, &ch, 1 ) == 1) {
	switch( *ch ) {
	    case  '\"': quotemode ^= 1;
			break;
	    case  ' ' :
	    case  '\t': if (quotemode) goto addit;
			break;

	    case  '=' : if (quotemode) goto addit;
			if (!mode) mode = 1;
			break;
	    case  '#' :
	    case  ';' : if (quotemode) goto addit;
			mode = 2;
			break;
	    case  '\n':
	    case  '\r': if (*name && *value)
			    new_init(name, value);
			*name = *value = quotemode = mode = 0;
			break;
	    default   :
addit:
			switch (mode ) {
			case 0 : strcat(name, ch);
				 break;
			case 1 : strcat(value, ch);
				 break;
			}
			break;
	}
    }
    _close(f );
    return( 0 );
}


void main(int argc, char *argv[])
{
  int i,send=0,recvmail=0,recvnews=0;
  char *directive, *value;

fprintf(stderr, "\n\nCCLXPOP 2.01  - Brian McIlvaine\n");
fprintf(stderr, "Brian.McIlvaine@unbounded.com\n");

initlists();
tcp_config();

  /* Parse the command line arguments */
for (i = 1; i < argc; i++)
	{
	directive = argv[i];
	if ((value = strchr(directive, '=')) != 0)
		{
		*value = 0;
		value++;
		strupr(directive);
		new_init(directive, value);
	}
	else if (!strcmpi(directive, "send")) send=1;
	else if (!strcmpi(directive, "recvmail" )) recvmail=1;
	else if (!strcmpi(directive, "recvnews")) recvnews=1;
	}

ReadFolders();

if (!(send||recvmail||recvnews))
	send=recvmail=recvnews=1;

  /* Complain if spooldir is not defined */
if (!spooldir)
	{
	fprintf(stderr, "No definition for cclxpop.spooldir\n");
	exit(0);
	}
       /* Complain if ccPath is not defined */
if (!ccPath)
	{
	fprintf(stderr, "No definition for cclxpop.ccpath\n");
	exit(0);
	}

  /* Complain if from is not defined */
if (!from)
	{
	fprintf(stderr, "No definition for cclxpop.from\n");
	exit(0);
	}

if (CheckSysMgr())   /*System Manager active */
	{
	check_ccmail();
	}

if (send)
	{
	i=MakeOutgoing();
	printf("Generated %i outgoing messages.\n",i);
	}
if (recvmail)
	{
	news=0;
	i=ThreadFile(inmail);
	printf("\nThreaded %i incoming email messages.\n",i);
	}

if (recvnews)
	{
	news=1;
	i=ThreadFile(innews);
	printf("\nThreaded %i incoming newsgroup messages.\n",i);
	}

}