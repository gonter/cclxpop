/* Functions common to both routines */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "cclxpop.h"


char *trim(char *str)
{ int i;

if (str == NULL) return(str);
for (i = strlen(str) - 1; (i >= 0) && isspace(str[i]);
	str[i--] = '\0');
while (isspace(str[0])) strcpy(str, str + 1);
return str;
}


char *getaddress(char *str)
{ char *p, *sp;

/* This does *very* rudimentary parsing. */
if ((p = strchr(str, '<')) != NULL)
	{
	/* Rod Whitby <rwhitby@iname.com> */
	sp = ++p;
	if (((p = strpbrk(sp, " \"()<>")) != NULL) && (*p == '>'))
		{
		*p = 0;
		return sp;
		}
	else { return str; }
	}
else if ((p = strpbrk(str, " \t\"(")) != NULL)
	{
	/* rwhitby@iname.com (Rod Whitby) */
	*p = 0;
	return str;
	}
else	{
	/* rwhitby@iname.com */
	return str;
	}
}


deflist list[NUMLIST];

void initlists(void)
{ int i;

for(i=0;i<NUMLIST;i++)
	list[i].head=list[i].tail=NULL;
}


void addlist( int which, char *str)
{  strlist *newptr=malloc(sizeof(strlist));

newptr->str=strdup(str);
newptr->next=NULL;

if (list[which].head==NULL)
	list[which].head=newptr;
else	list[which].tail->next=newptr;
list[which].tail=newptr;
}

void destroylist(int which)
{ strlist *t, *ptr=list[which].head;
while(ptr)
	{
	t=ptr->next;
	free(ptr);
	ptr=t;
	}
list[which].head=list[which].tail=NULL;
}
