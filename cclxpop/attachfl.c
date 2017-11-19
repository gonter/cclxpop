#include "cclxrecv.h"
#include <stdio.h>
#include <string.h>


char *getnextattname(void)
{ char fname[150];
  long t;
  FILE *f;

strcpy(fname,ccPath);
strcat(fname,"inbox.att");
/*printf("Opening %s\n",fname);*/
f=fopen(fname,"r+b");
if (f)
	{
	rewind(f);
	fread(&t,4,1,f);
	rewind(f);
	t+=1;
	fwrite(&t,4,1,f);
	}
else	{
	f=fopen(fname,"wb");
	t=1;
	fwrite(&t,4,1,f);
	}
fclose(f);
sprintf(fname,"%s%8.8lX.ATT",ccPath,t);
/*printf("Attachment file name: %s\n",fname);*/
return strdup(fname);
}

char zero24[24]={0,0,0,0,0,0,0,0,0,0,0,0,
		 0,0,0,0,0,0,0,0,0,0,0,0};

void AddAttachment(char *fname, char *fdesc)
{ long i,t=6;

msghdr1.attach++;
fwrite(&t,4,1,out);
i=strlen(fname)+strlen(fdesc)+2;
fwrite(&i,4,1,out);
fwrite(zero24,24,1,out);
fwrite(fdesc,strlen(fdesc)+1,1,out);
fwrite(fname,strlen(fname)+1,1,out);
fprintf(outbody,"\x0d\x0a<<Attachment: %s {ccMail: %s}>>\x0d\x0a\0xd\0xa",
	fdesc,fname);
if (verbose)
	printf("RECEIVED ATTACHMENT: %s Saved as: %s\n",fdesc,fname);
}
