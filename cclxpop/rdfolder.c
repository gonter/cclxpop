#include "cclxrecv.h"
#include <fcntl.h>
#include <dos.h>
#include <string.h>
#include <io.h>
#include <process.h>

/* Read_folders updates two global variables: NumFolders and FolderList
   to reflect the makeup of the folders in ccMail file inbox.fld. ccMail
   path is passed as argument*/
char *FolderList[200];
int NumFolders,FolderIndex[200];

void ReadFolders(void)
{ char fname [120];
  char foldbuff[100];
  int i,t,h;
strcpy(fname,ccPath);
strcat(fname,foldfile);
h=open(fname, O_RDONLY|O_BINARY);
if (h!=-1)
	{
	read(h,&NumFolders, 2);
	if (NumFolders>200)
		NumFolders=200;
	for(i=0;i<NumFolders;i++)
		{
		read(h,&FolderIndex[i],2); /*read and ignore index of folder*/
		read(h,&t,2); /*read folder name length*/
		read(h,foldbuff,t);
		FolderList[i]=strdup(foldbuff);
		}
	}
else	{
	printf("inbox.fld not found. ccPath=%s",fname);
	exit(-1);
	};
close(h);
}
