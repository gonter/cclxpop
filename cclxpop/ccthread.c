/* ccThread.c - Thread in the mail into ccMail. */
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <ctype.h>

#include "cclxpop.h"
#include "cclxrecv.h"


char buffer[BUFSIZE],boundary[256],*bp=buffer,*attname,*outattname;
int msgOk,bd,curfolder;
char marker[]={1,1,1,1};
char *Fields[NUMFIELDS]={"From:","To:","cc:","bcc:","Newsgroups:",
		"Subject:","Date:"};
/* From, To, cc, bcc, and newsgroup must be first entries to support
   use of list array*/
/*typedef enum def_states2 {FROM,TO,CC,BCC,NEWSGROUP,SUBJ,DATE,
	BODY,BASE64,NEEDFILENAME,BEGIN,HEADER}msgstate;   */

int Fieldlen[NUMFIELDS]={5,3,3,4,11,8,5};
char *hdrstr[NUMFIELDS];
msgstate msg;
FILE *in,*out,*outbody,*outatt,*outaddr,*outhdr;
long lastfilepos,fn;
adrec ad;
msghdr msghdr1;
ccm rec;
const char nullstr[]={1,0,0};

/* void PotentialError(void)
{
printf("Potential Error");
} */

long getnextfile(void)
{ int handle;
  char fname[120];
  long t;

strcpy(fname,ccPath);
strcat(fname,inbox);
/* printf("Opening %s\n",fname);*/
handle=open(fname,O_BINARY|O_RDWR);
if (handle!=-1)
	{
	read(handle,&t,4);
	lseek(handle,0,0);
	t++;
	write(handle,&t,4);
	close(handle);
	}
return(t);
}

char VIMMAIL[]={9,0,'V','I','M','_','M','A','I','L',0};
char outbodyname[160],outaddrname[160],outhdrname[160];
extern int _fmode;

void SetupOutFile(void)
{ char fname[250];
fn=getnextfile(); /*save in fn for updating ccm */
sprintf(fname,"%s%8.8X.msg",ccPath,fn);
out=fopen(fname,"w+b");
strcpy(outbodyname,ccPath);
_fmode=O_BINARY;
close(creattemp(outbodyname,0));
if (appendhdr)
	{
	strcpy(outhdrname,ccPath);
	close(creattemp(outhdrname,0));
	outhdr=fopen(outhdrname,"w+b");
	}
outbody=fopen(outbodyname,"w+b");
if (!out || !outbody || (appendhdr && !outhdr))
	{
	fprintf(stderr,"File error, aborting");
	exit(-1);
	}
msghdr1.j1=0xF0220001L;
msghdr1.j2=0x0000002dL;
msghdr1.header=0;
msghdr1.attach=1;
curfolder=0;
fwrite(&msghdr1,sizeof(msghdr),1,out);
fwrite(&VIMMAIL,sizeof(VIMMAIL),1,out);
/*msghdr will be updated later. Subject to be written at completion of
  header*/
/* date and size update at the end, msghdr will be rewritten to reflect
   updated header and attach values as well. */
}

long subjcode=0x0000003E;
char vimtext[20]="VIM_UNWRAPPED_TEXT";
char *HeaderSep=
"\x0d\x0a\x0d\x0a-----------HEADER INFORMATION-----------\x0d\x0a\x0d\x0a";


void CloseOutFile(void)
{ long bodylen,lz=0,n;
  char *subjattname;
  int i;
  size_t m;

/* if saving header info, append onto tail of body of message */
if (appendhdr)
	{
	/* now copy header info from outhdr */
	fprintf(outbody,HeaderSep);
	fseek(outhdr,0,0);
	while(!feof(outhdr))
		{
		m=fread(buffer,1,sizeof(buffer),outhdr);
		fwrite(buffer,1,m,outbody);
		}
	fclose(outhdr);
	remove(outhdrname);
	}

/* Put zero on end of outbody to null terminate string */
fwrite(&lz,1,1,outbody);

bodylen=ftell(outbody);
if (bodylen>20500)
	{
	/* too big, need to save to seperate file and */
	/* rename as next attachment */
	fclose(outbody);
	subjattname=getnextattname();
	rename(outbodyname,subjattname);
	/* add attachment name to filebody */
	AddAttachment(subjattname,"Message Body");
	free(subjattname);
	/* reopen outbody */
	outbody=fopen(outbodyname,"w+");
	/* write message in body */
	fprintf(outbody,"Message too long. Added as attachment.");
	bodylen=ftell(outbody);
	}
fseek(out,0,2);
fwrite(&subjcode,4,1,out);
fwrite(&bodylen,4,1,out);
fwrite(&lz,4,1,out);
fwrite(vimtext,1,20,out);

/* now copy message body onto end */
fseek(outbody,0,0);
while (!feof(outbody))
	{
	m=fread(buffer,1,sizeof(buffer),outbody);
	fwrite(buffer,1,m,out);
	}
fclose(outbody);
remove(outbodyname);
msghdr1.size=ftell(out)+255; /* Put some more here, ftell(out?)*/
fseek(out,0,0);
fwrite(&msghdr1,sizeof(msghdr),1,out);
fclose(out);
SelectFolder();
/* now update inbox.ccm to reflect this file */
strcpy(outbodyname,ccPath);
strcat(outbodyname,inbox);
out=fopen(outbodyname,"r+b");
if (out)
	{
	fread(&n,4,1,out); /* get lead in message number */
	fseek(out,0,2); /* skip to end of file */
	rec.unk1=1;
	rec.unk2=0xF022;
	if (curfolder)
		{
		rec.unk3=fn;
		rec.fileref=n++;
		rec.folder=curfolder;
		}
	else	{
		rec.unk3=0;
		rec.fileref=fn;
		rec.folder=0;
		}
	if (verbose==1)
		{
		printf("FOLDER: %i\n",curfolder);
		}
	rec.unk4=0x2D;
	rec.day=msghdr1.day;
	rec.month=msghdr1.month;
	rec.year=msghdr1.year;
	rec.hund=msghdr1.hund;
	rec.sec=msghdr1.sec;
	rec.min=msghdr1.min;
	rec.hour=msghdr1.hour;
	fwrite(&rec,sizeof(ccm),1,out);
	if (curfolder)
		{
		rec.unk3=1;
		rec.folder=0;
		rec.fileref=fn;
		fwrite(&rec,sizeof(ccm),1,out);
		fseek(out,0,0);
		fwrite(&n,4,1,out);
		}
	fclose(out);
	}
else	{
	printf("Unable to open %sinbox.ccm. Aborting");
	exit(-1);
	}
/* Free up memory from hdrstr[]*/
for(i=HDRSTART;i<HDREND;i++)
	if (hdrstr[i])
		{
		free(hdrstr[i]);
		hdrstr[i]=0;
		}
/*Destroy Lists*/
for (i=0;i<NUMLIST;i++)
	destroylist(i);
}

void PutBuffer(void)
{ char *tptr;

tptr=strchr(buffer,0)-1;
if (*tptr==10)
	{
	*tptr=13;tptr++;
	*tptr=10;tptr++;
	*tptr=0;
	}
fwrite(buffer,strlen(buffer),1,outbody);
}


char Contentcode[]="Content-Transfer-Encoding:";

int ThreadFile(char *infile)
{ char fname[120],*tptr;
  int msgcount=0,i;

strcpy(fname,spooldir);
strcat(fname,infile);
in=fopen(fname,"rt");
if (!in)
	return 0;
msg=BEGIN;
while (!feof(in))
	{
	fgets(buffer,sizeof(buffer),in);
REENTER:
	switch(msg)
		{
		case BEGIN:
			if (strncmp(buffer,marker,sizeof(marker))==0)
				{
				msg=HEADER;
				/* Establish conditions for new file */
				SetupOutFile();
				}
			break;
		case HEADER:
			for (i=HDRSTART;i<HDREND;i++)
				if (strnicmp(buffer,Fields[i],Fieldlen[i])==0)
				{
				msg=i;
				hdrstr[i]=strdup(trim(&buffer[Fieldlen[i]]));
				break;
				};
			if (appendhdr)
				fprintf(outhdr,"%s\x0d\x0a",buffer);
			if (strlen(trim(buffer))==0)
				{
				msg=BODY;
				WriteAddrInfo();
				}
			break;

		case TO:
		case FROM:
		case CC:
		case BCC:
		case SUBJ:
		case NEWSGROUP:
			if (appendhdr)
				fprintf(outhdr,"%s\x0d\x0a",buffer);
			if (isspace(buffer[0]))
				{
				if (strlen(trim(buffer))==0)
					{
					if (msg!=SUBJ)
						ParseAddrInfo(msg);
					msg=BODY;
					WriteAddrInfo();
					}
				else
					{
					/* Continue field */
					tptr=malloc(strlen(hdrstr[i])+strlen(trim(buffer))+2);
					strcpy(tptr,hdrstr[i]);
					strcat(tptr,buffer);
					free(hdrstr[i]);
					hdrstr[i]=tptr;
					}
				}
			else	{
				/* New Field, end previous and reenter*/
				if (msg!=SUBJ)
					ParseAddrInfo(msg);
				msg=HEADER;
				goto REENTER;
				}
			break;
		case DATE:
			ParseDate(hdrstr[DATE]);
			msg=HEADER;
			goto REENTER;
		case BODY:
			if (strncmp(buffer,marker,sizeof(marker))==0)
				{
				CloseOutFile();
				msg=BEGIN;
				msgcount++;
				printf("MESSAGE #%i completed.\r",msgcount);
				}
			else	{
				/* put in outbody*/
	i=strnicmp(buffer,Contentcode, sizeof(Contentcode)-1);
				if (i==0)
					{
if (strustr(&buffer[sizeof(Contentcode)],"BASE64"))
						msg=NEEDFILENAME;
						attname=NULL;
					}
				PutBuffer();
				};
			break;
		case NEEDFILENAME:
			PutBuffer();
			tptr=strstr(buffer,"name=");
			if (tptr && !attname)
				{
				tptr=strchr(tptr,'\"')+1;
				*strchr(tptr,'\"')=0;
				attname=strdup(tptr);
				}
			if (strlen(trim(buffer))==0)
				{
				/*open attachment file*/
				outattname=getnextattname();
				outatt=fopen(outattname,"w+b");
				if (!outatt)
					{
fprintf(stderr,"File error while opening attachment file. Aborting");
					exit(-1);
					}
				else msg=BASE64;
				}
			break;
		case BASE64:
			/*Base64 decode*/
			if (!attname)
				attname=strdup("default");
			GetAttach64();
			msg=BODY;
			break;
		}
	}
/* close out current message, if applicable */
/*close out input file */
fclose(in);
return msgcount;
}
