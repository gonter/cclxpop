/* resolve_address takes two arguments: int outfile is the handle for the
   designated output file. char *name is the address that is an address that
   does not include an "@" (standard email) and does not match a recognized
   newsgroup folder. */

#include <string.h>
#include <stdio.h>
#include <mem.h>
#include "cclxpop.h"

extern char *spooldir;
extern char *trim(char*);

struct def_idx {
char hash[11],code;
long offset;
} idx;


/* InAddrBook checks to see if name is in the address book, and if so if it
   is a list or single entry. Returns: 0 = not in book, 1= single 2=list */

int InAddrBook(char *name, char *resolved)
{ char fn1[100],fn2[100];
  FILE *f1,*f2;
  char locbuff[200],revbuff[200],*rb,*em;
  int i=0,l_str,k=strlen(name);


if (strchr(name,'@'))
	{
	strcpy(resolved,getaddress(name));
	return 0;
	};
strcpy(locbuff,name);
strcpy(fn1,ccPath);
strcpy(fn2,ccPath);
strcat(fn1,"vim_priv.idx");
strcat(fn2,"vim_priv");
f1=fopen(fn1,"rb");
f2=fopen(fn2,"rb");
if (f2 && f1)
	{
	fseek(f1, 0x16, SEEK_SET);
	while (!feof(f1) &&!i)
		{
		fread(&idx,sizeof(idx),1,f1);
		if (idx.code==0x45 || idx.code==0x47)
		/* Name Code or list code */
			{
			fseek(f2, idx.offset+42, SEEK_SET);
			fread(&l_str, 2, 1, f2);
			if (l_str<sizeof(locbuff))
				{
				fread(locbuff, l_str, 1, f2);
				/* Take care of comma on name */
				em=locbuff+strlen(locbuff)+5;

				rb=strchr(locbuff,',');
				if (rb)
					{
					*rb=0;
					strcpy(revbuff,trim(rb+1));
					strcat(revbuff," ");
					strcat(revbuff,locbuff);
					strcpy(locbuff,revbuff);
					trim(locbuff);
					}

				if (strncmp(locbuff,name,k)==0)
					{
					strcpy(resolved,em);
					if (idx.code==0x45) i=1;
					else i=2;
					}
				}
			}
		}
	fclose(f2);
	fclose(f1);
	}
if (i==0)
	strcpy(resolved, getaddress(name));
return i;
}


void resolve_address(FILE *outfile, char *name, int *first)
{ char fn1[100],fn2[100],matchlist[11];
  FILE *f1,*f2;
  char locbuff[200], revbuff[60], *rb,*lb=locbuff,*em,*em1;
  int l_str;
  int listfound=0;
  long listoffset=0,testoffset;

if (strchr(name,'@'))
	{
	if (!*first)
		fprintf(outfile,",\n\t");
	else	*first=0;
	fprintf(outfile,"%s",name);
	return;
	};
strcpy(locbuff,name);
strcpy(fn1,ccPath);
strcpy(fn2,ccPath);
strcat(fn1,"vim_priv.idx");
strcat(fn2,"vim_priv");
f1=fopen(fn1,"rb");
f2=fopen(fn2,"rb");
if (f2 && f1)
	{
	fseek(f1, 0x16, SEEK_SET);
	while (!feof(f1))
		{
		fread(&idx,sizeof(idx),1,f1);
if (listfound)
	{
	if ((idx.code==0x4d) && strncmp(idx.hash,matchlist,11)==0)
		{
		fseek(f2,idx.offset+42,SEEK_SET);
		fread(&l_str,2,1,f2);
		fread(&testoffset,4,1,f2);
		if (l_str<sizeof(locbuff) && testoffset==listoffset)
			{
			fread(locbuff, l_str,1,f2);
			em=locbuff+strlen(locbuff)+4;
			if (!*first) /* need to put ; at end of name*/
				fprintf(outfile,",\n\t");
			else *first=0; /* found first entry */
			/* Strip off at Internet if present */
			em1=strchr(em,' ');
			if (em1)
				*em1=0;
			fprintf(outfile,"<%s>",em);
			}
		}
	}
else if (idx.code==0x45 || idx.code==0x47) /* Name Code */
	{
	fseek(f2, idx.offset+42, SEEK_SET);
	fread(&l_str, 2, 1, f2);
	if (l_str<sizeof(locbuff))
		{
		fread(locbuff, l_str, 1, f2);
		if (idx.code==0x45)
			{
			em=locbuff+strlen(locbuff)+5;
			rb=strchr(locbuff,',');
			if (rb)
				{
				*rb=0;
				strcpy(revbuff,trim(rb+1));
				strcat(revbuff," ");
				strcat(revbuff,locbuff);
				strcpy(locbuff,revbuff);
				lb=trim(locbuff);
				}
			if (!strncmpi(lb,name,strlen(lb)))
				{
				if (!*first) /* need to put , at end of name*/
					fprintf(outfile,",\n\t");
				else *first=0; /* found first entry */
				/* Strip off at Internet if present */
				em1=strchr(em,' ');
				if (em1)
					*em1=0;
				fprintf(outfile,"\"%s\"<%s>",lb,em);
				}
			}
		else if (idx.code==0x47)
			{
			if (!strncmpi(locbuff,name,strlen(name))) /* Found List Title Match */
				{
				movmem(idx.hash,matchlist,11);
				listoffset=idx.offset;
				listfound=1;
				rewind(f1);
				fseek(f1,0x16,SEEK_SET);
				}
			}
		}
	}
		}
	fclose(f2);
	fclose(f1);
	}
}
