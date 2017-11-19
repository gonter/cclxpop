#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <fcntl.h>
#include <dir.h>
#include <conio.h>
#include <stdlib.h>
#include <io.h>

#include "cclxpop.h"

/* Last_Outgoing returns an integer equal to the last outgoing file number
   in the LXMTA spool directory. It returns -1 if there are no outgoing
   files. Call with 'm' for mail or 'n' for newsgroup files. */

int LastOutgoing(char mn)
{  char fname[120];
   struct ffblk ffblk;
   int done=0,last=-1,t;

sprintf(fname,"%soutgoing.%c??",spooldir,mn);
done = findfirst(fname,&ffblk,0);
while (!done)
	{
	t=atoi(&ffblk.ff_name[10]);
	if (t>last) last=t;
	done = findnext(&ffblk);
	}
return last;
}



/* MakeOutgoing makes a new file for each outgoing message for
   passing to LXMTA. This routine checks for existence of outgoing.m??
   to see what the next file number is. */


int MakeOutgoing(void)
{ int h,result,msgcount=0;
  char fname[120], fn[13];
  long filepos;
  ccm cc;

strcpy(fname,ccPath);
strcat(fname,inbox);
h=open(fname, O_RDWR|O_BINARY);
if (h!=0)
	{
	read(h,&cc,4); /* read in next file number and trash-don'tneed it*/
	filepos=lseek(h,0L,1);
	while (!eof(h))
		{
		filepos=lseek(h,0L,1);
		read(h,&cc,sizeof(ccm));
		if (cc.folder==outboxfolder)
			{
			sprintf(fn,"%8.8X.MSG",cc.unk3);
			result=GenerateFile(fn);
			if (result!=-1)
			      {
			      cc.folder=sentfolder;
			      lseek(h,filepos,0);
			      write(h,&cc,sizeof(ccm));
			      msgcount++;
			      }
			};
		}
	}
close(h);
return msgcount;
}
