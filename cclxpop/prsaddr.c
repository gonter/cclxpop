#include "cclxpop.h"
#include "cclxrecv.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <mem.h>

extern char zero24[];
extern char *hdrstr[];
extern deflist list[];

int tvalarray[]={CCFROM,CCTO,CCCC,CCBCC,CCCC};


void post_adr2(int which, char *t)
{ int i;
ad.settoone=1;
ad.set1a=0x1A;
ad.j2=0x0B;
ad.j3=0;
ad.type=tvalarray[which];
ad.length=strlen(t)+1;
movmem(zero24,ad.k,24);
ad.len1=ad.length+14;
fwrite(&ad,sizeof(adrec),1,out);
fwrite(t,1,strlen(t)+1,out);
fwrite(&nullstr,3,1,out);
msghdr1.header++;
if (verbose==1)
	printf("ADDR: %s type =%s\n",t,(int)Fields[which]);
if (!curfolder && news)
	for(i=0;i<NumFolders;i++)
		if (strstr(t,FolderList[i]))
			curfolder=FolderIndex[i];
}

char *trimfront(char *str)
{ char *str2=str;
while (isspace(*str2)) str2++;
strcpy(str, str2);
return str;
}

char *trimback(char *str)
{ char *str2=strchr(str,0)-1;
while (isspace(*str2) && str2>str)
	{
	*str2=0;
	str2--;
	}
return str;
}

void ParseAddrInfo(int which)
{ char *s,*t,*i;
  int singleqt=0,doubleqt=0,j;

s=hdrstr[which];
t=strchr(s,0);
while (s<t)
	{
	for (i=s;i<t;i++)
		switch (*i)
			{
			case '\'':
				if (singleqt)
					singleqt=0;
				else	singleqt=1;
				break;
			case '\"':
				if (doubleqt)
					doubleqt=0;
				else	doubleqt=1;
				break;

			case ',':
				if (!singleqt && !doubleqt)
					{
					*i=0;
					addlist(which,trim(s));
					s=i+1;
					}
				break;
			case 0:	break;
			};
	addlist(which,trim(s));
	if (which==NEWSGROUP)
	if (curfolder==0)
		{
		for(j=0;j<NumFolders;j++)
			if(strcmp(s,FolderList[j])==0)
				{
				curfolder=FolderIndex[j];
				break;
				}
		}
	s=i+1;
	}
}

void WriteAddrInfo(void)
{ int i;
  strlist *ptr;

/* add in Subject Line First */
i=strlen(hdrstr[SUBJ])+1;
fwrite(&i,2,1,out);
fwrite(hdrstr[SUBJ],1,i,out);
/* now copy address records from lists */
/* ccMail expects specific order to be From:, To:, CC, BCC for email*/
/* Fudge the order a bit for news, if necessary*/
for(i=0;i<5;i++)
	{
	if (news)
		{
		if (i==TO)
			ptr=list[NEWSGROUP].head;
		else if (i==NEWSGROUP)
			ptr=list[TO].head;
		else	ptr=list[i].head;
		}
	else
		ptr=list[i].head;
	while(ptr)
		{
		if (strlen(ptr->str)>0)
			post_adr2(i,ptr->str);
		ptr=ptr->next;
		if (i==FROM) break; /*prevent ccMail error*/
		}
	}
}