#include "cclxrecv.h"
#include <string.h>
#include <ctype.h>


char boundmark[]="BOUNDARY=";

void ParseBoundary(void)
{ int done=0;
  char *t,*s;

while (!done)
	{
	t=strustr(buffer, boundmark);
	if (t)
		{
		s=t+sizeof(boundmark);
		boundary[0]=0;
		strcpy(boundary,"--");
		s=trim(s);
		strcat(boundary,s);
		t=strchr(boundary,'\"');
                if (t) *t=0;
		done=1;
		}
	LoadBuffer();
	if (isspace(buffer[0]))
		{
		/* either end of header, or continuation of body */
		if (strlen(t=trimfront(buffer))==0)
			{
			msg=BODY ;  /* End of Header */
			done=1; /*  force exit from while loop */
			}
		else 	{
			/* nothing - just reloop and concat string onto s*/
			}
		}
	else	{ /* done with current, evaluate new header */
		msg=HEADER; /* Still is, really */
		ResetFilePos(); /* reprocess current line */
		done=1; /* force exit */
		}
	}
}


void HandleHeader(void)
{ int i,j;
if (strlen(trim(buffer))==0)
	msg=BODY;
else	{
	for(i=2;i<NUMFIELDS;i++)
		{
		j=(strnicmp(Fields[i],buffer,strlen(Fields[i])));
		if (j==0)
			break;
		}
	switch (i)
		{
		case TO:
			if (news)
			/* if newsgroup To:, then add to cc Field to prevent
			   having two "To:" Fields, which ccMail bombs on*/
				ParseAddress(CC);
			else
				ParseAddress(TO);
			break;
		case FROM:
		case CC:
		case BCC:
			ParseAddress(i);
			break;
		case DATE:
			/*parse date, put in msghdr*/
			ParseDate();
			break;
		case SUBJ:
			/* Parse subject then write in out file */
			ParseSubj();
			WriteSubj();
			break;
		case CONTENT:
			ParseBoundary();
			break;
		case NEWSGROUP:
			ParseAddress(NEWSGROUP);
			break;

		default:
			break;
			/* junk header field, save if save header set,
			else skip to end of this entry */
		}
	}
}
