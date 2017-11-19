#include "cclxpop.h"
#include "cclxrecv.h"
#include <string.h>

extern char *hdrstr[];
extern deflist list[];


void SelectFolder(void)
{  FILE *r;
   int i,j;
   char *Param,*Field,*Folder;
   strlist *ptr;
/* Look for folder selection file */
if (curfolder)
	return;
r=fopen("cclxpop.fld","rt");
if (!r)
	return;
while (!feof(r) && curfolder==0)
	{
	fgets(buffer,sizeof(buffer),r);
	Field=buffer;
	Param=strchr(buffer,',');
	Folder=strchr(Param+1,',');
	if (Param && Folder)
		{
		*Param=0;Param++;
		*Folder=0;Folder++;
		strupr(trim(Param));
		trim(Field);
		trim(Folder);
		/* Match Field */
		for(i=0;i<HDREND;i++)
			if (stricmp(Field,Fields[i])==0)
				break;
		if (i==HDREND) break; /* Not a valid Field */
		/* See if it is a valid folder */
		for(j=0;j<NumFolders;j++)
			if (strnicmp(FolderList[j],Folder,
				strlen(FolderList[j]))==0)
				break;
		if (j==NumFolders) break; /* No such folder, next search */
		/* Now see if it is in the list */
		switch (i)
			{
			case TO:
			case FROM:
				ptr=list[i].head;
				while(ptr)
					{
					if (strustr(ptr->str,Param))
						{
						curfolder=FolderIndex[j];
						break;
						};
					ptr=ptr->next;
					}
				break;
			case SUBJ:
				if (strustr(hdrstr[i],Param))
					{
					curfolder=FolderIndex[j];
					}
				break;
			}
		}
	}
fclose(r);
}
