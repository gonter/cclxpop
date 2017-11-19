#include "cclxpop.h"
#include <stdio.h>
#include <conio.h>
#include <dir.h>


ccm rec;


void main(void)
{ FILE *in;
  long n;
  int i;
  char *dir=getcwd(NULL,255),fname[255];

sprintf(fname,"%s\\inbox.ccm",dir);
printf("Dumping File: %s\n",fname);

in=fopen(fname,"rb");

i=fread(&n,4,1,in);
printf("Next file number: %8.8LX\n",n);
printf(
"Folder unk1 unk2 unknown3 fileref  unknown4  YYYYMMDD HH:MM:SS.HH\n");

while(!feof(in))
	{
	i=fread(&rec,sizeof(ccm),1,in);
if (i)	{
	printf("%6i ",rec.folder);
	printf("%4X ",rec.unk1);
	printf("%4X ",rec.unk2);
	printf("%8LX ",rec.unk3);
	printf("%8LX ",rec.fileref);
	printf("%8LX ",rec.unk4);
	i=rec.year;
	printf("%i ",i);
	printf("%i ",(int)rec.month);
	printf("%i ",(int)rec.day);
	printf("  %i:",(int)rec.hour);
	printf("%i:",(int)rec.min);
	printf("%i.",(int)rec.sec);
	printf("%i\n",(int)rec.hund);
	}
	}
}

