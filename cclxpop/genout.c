#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <fcntl.h>
#include <time.h>

#include "cclxpop.h"
#include "cclxrecv.h"

extern int news;
char outbuffer[500];
extern char buffer[BUFSIZE];

/* GenerateFile is passed the file name of the 0000000.MSG file to be read.
   It determines whether it is a mail message or an output file and generates
   the appropriate outgoing.m00 or outgoing.n00 message. If the message is
   successfully created and savesent is true, it returns 1. If the message is
   to be deleted, it returns 0. If the message wasn't generated, it returns
   -1.
*/

int ComputeDay(int date, int month, int year)
{ int day,n1,n2;

if (month<3)
	{
	month+=12;
	year -=1;
	}
n1= (26*(month+1))/10;
n2= (int) ((125 * (long) year) /100);
day= ((date+n1+n2-(year/100)+(year/400)-1 ) %7);
return day;
}

char *Weekdays[]={"SUN","MON","TUE","WED","THU","FRI","SAT"};

void SendAddresses(FILE *sourcefile, FILE *out,long addrstart, int n,
			char adrtype)
{  int i,first=1;
   adrec a;

/* Search through the address records in the current file and send those
   that match type. Make sure to use ";" and returns correctly. */
fseek(sourcefile,addrstart,0);
for(i=0;i<n-1;i++)
	{
	fread(&a,sizeof(adrec),1,sourcefile);
	fread(buffer,a.length+3,1,sourcefile);
	if (a.type==adrtype)
		{
		if (first)
			{
			if (adrtype==CCTO)
				fprintf(out,"To:\t");
			else if (adrtype==CCCC)
				fprintf(out,"cc:\t");
			else if (adrtype==CCBCC)
				fprintf(out,"bcc:\t");
			resolve_address(out,buffer,&first);
			first=0;
			}
		else	{
			resolve_address(out,buffer,&first);
			}
		}
	}
if (!first) /* at least one address was printed, so print LF */
	fprintf(out,"\n");
}

int validnewsgroup(char *t)
{
return (strchr(t,'.') && !strchr(t,'@') && !strchr(t,' '));
}

void SendNewsAddresses(FILE *sourcefile, FILE *out,long addrstart, int n)
{  int i,first=1;
   adrec a;

/* Search through the address records in the current file and send those
   that match type. Make sure to use ";" and returns correctly. */
fseek(sourcefile,addrstart,0);
for(i=0;i<n-1;i++)
	{
	fread(&a,sizeof(adrec),1,sourcefile);
	fread(buffer,a.length+3,1,sourcefile);
	if (a.type==CCTO && validnewsgroup(buffer))
		{
		if (first)
			{
			fprintf(out,"Newsgroups: %s",buffer);
			first=0;
			}
		else	{
			fprintf(out,",\n\t%s",buffer);
			}
		}
	}
if (!first) /* at least one address was printed, so print LF */
	fprintf(out,"\n");
}



void FileAbort(void)
{
printf("File error!!! (Enough File Handles Available?)\n");
exit(-1);
};

int GenerateFile(char *sourcefile)
{ int i,t=1,s,n;
  size_t nr;
  long addrstart,l,msg_start,att_addr,k;
  char fname1[120], *subj,*attdescptr,*attfileptr;
  char boundary[40],*r,*sn;
  msghdr m;
  adrec a;
  FILE *in,*out,*sigf;

news=1; /* default condition until (easily) shown otherwise */
strcpy(fname1,ccPath);
strcat(fname1,sourcefile);
in=fopen(fname1,"rb");
if (in)
	{
	/* First determine if it is email or newsgroup posting. Default
	assumption is email... It will be sent to newsgroup only if the
	following are true: (1) There must be exactly one addressee.
	(2) After resolving addressee (check for address list entry), if the
	address contains at least one '.' and NO '@', it will be considered
	a newsgroup and so posted. */
	/* Read in header, */
	fread(&m,sizeof(msghdr),1,in);
	/* read in subject size */
	fseek(in,11,SEEK_CUR); /* skip 09 00 VIM_MAIL */
	fread(&s,2,1,in);	/* read in subject size */
	subj=(char *) malloc(s);
	fread(subj,s,1,in);
	/* read in first address record */
	addrstart=ftell(in);
	fread(&a,sizeof(adrec),1,in);
	fread(buffer,a.length+3,1,in);
	/* Generate either .m00 or .n00 extension as required, then */
	/* Write either "To:" or "Newsgroups:" field as appropriate */

	if (news)
		{
		if (InAddrBook(buffer,outbuffer)<2)
			{
			if (!validnewsgroup(buffer))
				news=0;
			}
		else news=0;
		}
	if (news)
		{
		/* open file with n?? extension */
		n=LastOutgoing('n')+1;
		if (n>99)
			FileAbort();
		sprintf(buffer,"%soutgoing.n%2.2i",spooldir,n);
		out=fopen(buffer,"wt");
		if (!out)
			{
			/* handle the error condition */
			FileAbort();
			}
		SendNewsAddresses(in,out,addrstart,m.header);
		}
	else	{
		n=LastOutgoing('m')+1;
		if (n>99)
			FileAbort();
		sprintf(buffer,"%soutgoing.m%2.2i",spooldir,n);
		out=fopen(buffer,"wt");
		if (!out)
			{
			/* handle the error condition */
			FileAbort();
			}

		SendAddresses(in,out,addrstart,m.header,CCTO);  /* send addition "to's", if needed*/
		SendAddresses(in,out,addrstart,m.header,CCCC);
		SendAddresses(in,out,addrstart,m.header,CCBCC);
		}

	fseek(in,4,SEEK_CUR); /* skip 0x3e long*/
	msg_start=ftell(in)+28;    /*save message start point */
	fread(&l,4,1,in); /* Size of outgoing message body*/
	fseek(in,24+l,SEEK_CUR); /* Jump to attachments*/
	att_addr=ftell(in);
	/*  Read past attachments */
	for(i=0;i<m.attach-1;i++)
		{
		fread(&k,4,1,in); /* Skip lead in descriptor word */
		if (k!=6) {}
		else	{
			/* read Att description and file size total */
			fread(&s,2,1,in);
			fseek(in,26,SEEK_CUR);
			fread(buffer,1,s,in);
			}
		};
	/* Write Date / Time */
	/* date field: format: "Date: Mon, 23 Oct 1981 11:22:33 -0400" */
	fprintf(out,"Date:\t %s, %i %s %2.2i %2.2i:%2.2i:%2.2i %4.4i\n",
		Weekdays[ComputeDay(m.day,m.month,m.year)], m.day,
		months[m.month-1],m.year, m.hour,m.min,m.sec,timezone);
	/* Write From / Reply-To */
	fprintf(out,"From: %s\n",from);
	/* Write Subject */
	fprintf(out,"Subject: %s\n",subj);
	free(subj);
	/* Write Message ID */
	/* Write MIME stuff */
	if (!news)
		fprintf(out,"Message-ID:\t<cclxpop_20_%s>\n",sourcefile);
	fprintf(out,"MIME-Version: 1.0\n");
	if (m.attach-1)
		{ /*Setup multipart message */
		sprintf(boundary,"CCLXPOP__%8.8lX",time(NULL));
		fprintf(out,
			"Content-Type: multipart/mixed; boundary=\"%s\"\n",
			boundary);
		fprintf(out,"--%s\n",boundary);
		}
	fprintf(out,"Content-type: text/plain; charset=us-ascii\n");
	fprintf(out,"Content-Transfer-Encoding: 7bit\n");
	fprintf(out,"\r\n");

	/* Now send the body of the message */
	fseek(in,msg_start,SEEK_SET);
	/* l earlier set to body length by fread */
	while (l>0 && !feof(in))
		{
		nr=fread(buffer,1,sizeof(buffer),in);
		if (nr>l)
			nr=(size_t)l;
/*		if (strlen(buffer)<l)
			nr=strlen(buffer)+1;
*/
		fwrite(buffer,1,nr,out);
		l-=nr;
		}
	/* Now Send Signature File */
	if (sigfile)
		{
		sigf=fopen(sigfile,"rt");
		if (sigf)
			{
			fprintf(out,"\n");
			while (!feof(sigf))
				{
				n=fread(buffer,1,sizeof(buffer),sigf);
				fwrite(buffer,1,n,out);
				}
			}
		fclose(sigf);
		}
	/* now send attachments */
	fseek(in,att_addr,0);

	for(i=0;i<m.attach-1;i++)
		{
		fread(&k,4,1,in); /* Skip lead in descriptor word */
		if (k!=6) {}
		else	{
			/* Send splitter */
			fprintf(out,"\n\n--%s\n",boundary);

			/* read Att description and file size total */
			fread(&s,2,1,in);
			fseek(in,26,SEEK_CUR);
			attdescptr=(char *)malloc(s);
			fread(attdescptr,s,1,in);
			r=sn=attfileptr=attdescptr+strlen(attdescptr)+1;
			while (sn)
				{
				/* printf("Parsing %s\n",sname+1);wait(); */
				r=sn+1;
				sn=strchr(sn+1,'\\');
				}
			/* printf("Decomposed filename to %s\n",rname);*/
			fprintf(out,
"Content-Type: application/octet-stream; name=\"%s\"\n",r);
			fprintf(out,
"Content-Transfer-Encoding: base64\n");
			fprintf(out,
"Content-Disposition: attachment;\n\t filename=\"%s\"\n\n",r);

			/* Now send the file */
			send_base64(out,attfileptr);
			free(attdescptr);
			}
		}
	if (m.attach-1)
		{
		fprintf(out,"\n--%s--\n\n",boundary);
		}
	fclose(out);
	}
else	{
	t=-1;
	}
fclose(in);
return t;
}