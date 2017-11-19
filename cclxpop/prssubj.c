#include "cclxrecv.h"
#include <string.h>
#include <ctype.h>

char subjbuff[5000];

void ParseSubj(void)
{ int done=0;
  char *s=subjbuff;
  char *t=trimfront(strchr(buffer,':')+1);

*s=0;
while (!done)
	{
	strcat(s,t);
	LoadBuffer();
	/* Check for 1,1,1,1 marker????? */
	if (isspace(buffer[0]))
		{
		/* either end of header, or continuation of body */
		if (strlen(t=trim(buffer))==0)
			{
			msg=BODY ;  /* End of Header */
			done=1; /*  force exit from while loop */
			}
		else 	{
			/* had to sperate at a blank, so put in a space*/
			strcat(s," ");
			}
		}
	else	{ /* done with current, evaluate new header */
		msg=HEADER; /* Still is, really */
		ResetFilePos(); /* reprocess current line */
		done=1; /* force exit */
		}
	}
if (verbose==1)
	printf("SUBJ: %s\n",subjbuff);
}

void WriteSubj(void)
{ int i=strlen(subjbuff)+1;
fwrite(&i,2,1,out);
fwrite(subjbuff,1,i,out);
}