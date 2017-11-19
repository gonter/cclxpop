#include "a:\newcclx\cclxpop.h"
#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <string.h>
#include <ctype.h>


int sort_function( const void *a, const void *b)
{
return (*(long *)a-*(long *)b);
}




void main(int argc, char *argv[])
{ FILE *f,*out;
  char c,*junk,*dir;
  long t,list[10000];
  char fname[120],inbox[120];
  struct ffblk ffblk;
  int done=0,r,i=0,j=argc;
  ccm rec;
  msghdr hdr;

printf ("\nRebuilding inbox.ccm... \n");
dir=getcwd(NULL,255);
printf("Search Path: %s\\*.msg\n", dir);
sprintf(fname,"%s\\*.msg",dir);
done = findfirst(fname,&ffblk,0);
if (done)
	{
	printf("No files of type .msg found in path \"%s\"\n",argv[1]);
	exit(-1);
	}
sprintf(inbox,"%s\\inbox.ccm",dir);
out=fopen(inbox,"wb");
if (!out)
	{
	printf("Unable to open %s\n",inbox);
	exit(-1);
	}
printf("This program will put all files with .msg suffix into the inbox.\n");
printf("Press Y to continue...");
c=toupper(getche());
if (c!='Y')
	{
	fclose(out);
	exit(-1);
	}
printf("\nGenerating list of .msg files... ");
while (!done)
	{
	sprintf(fname,"%s\\%s",dir,ffblk.ff_name);
	f=fopen(fname,"rb");
	t=0;
	r=fread(&hdr,sizeof(msghdr),1,f);
	if (r==1)
		{
		*strchr(ffblk.ff_name,'.')=0;
		t=strtol(ffblk.ff_name,&junk,16);
		if (t!=0)
			list[i++]=t;
		}
	fclose(f);
	done = findnext(&ffblk);

	}
printf(" %i files found.\n",i);
printf("Sorting files...");
qsort(list, i, 4, sort_function);
t=list[i-1];
fwrite(&t,4,1,out); /*save last file number */
rec.folder=0;
rec.unk1=1;
rec.unk2=0xF822;
rec.unk3=0;
rec.unk4=0x2d;

for (j=0;j<i;j++)
	{
	sprintf(fname,"%s\\%8.8LX.msg",dir,list[j]);
	f=fopen(fname,"rb");
	r=fread(&hdr,sizeof(msghdr),1,f);
	if (r==1)
		{
		rec.folder=0;
		rec.unk1=1;
		rec.unk2=0xF822;
		rec.unk3=0;
		rec.unk4=0x2d;
		rec.fileref=list[j];
		rec.year=hdr.year;
		rec.month=hdr.month;
		rec.day=hdr.day;
		rec.hour=hdr.hour;
		rec.min=hdr.min;
		rec.hund=hdr.hund;
		rec.sec=hdr.sec;
		fwrite(&rec,sizeof(ccm),1,out);
		printf("Added %8.8LX.MSG to index file\n",list[j]);
		}
	fclose(f);
	}
fseek(out,0,0);
fwrite(&t,4,1,out);
fclose(out);
}


