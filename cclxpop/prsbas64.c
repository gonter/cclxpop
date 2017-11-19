#include "cclxrecv.h"
#include <mem.h>
#include <string.h>
#include <process.h>
#include <alloc.h>

const char db64[256]={
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

char residue64[15]="";

int DecodeOctets(void)
{ int i,done=0,r,k,j;
  char fb[4];
  char *s=buffer;

j=strlen(s);
k=strlen(residue64);
if (k)
	{
	movmem(buffer,&buffer[k],strlen(buffer));
	movmem(residue64,buffer,k);
	residue64[0]=0;
	}
while (strlen(s)>3 ||j==0)
	{
	for(i=0;i<4;i++,s++)
		{
		r=db64[*s];
		if (r>=0)
			{
			switch (i)
				{
				case 0: fb[0]=r<<2;
					break;
				case 1: fb[0]|=r>>4;
					fb[1]=r<<4;
					break;
				case 2:	fb[1]|=r>>2;
					fb[2]=r<<6;
					break;
				case 3: fb[2]|=r;
					fwrite(fb,1,3,outatt);
				}
			}
		else 	{
			if (*s=='=')
				{ /*fill in longbuffer and write*/
				switch (i)
					{
					case	2:
						fwrite(fb,1,1,outatt);
						break;
					case	3:
						fwrite(fb,1,2,outatt);
					}
				s[1]=0;j=1; /* force while loop exit */
				done=2;
				}
			else	{
				return 1;
				/*bad base 64 syntax*/
				}
			}
		}
	}
i=(strlen(s));
if (i>0)
	{
	strncpy(residue64,s,3);
	}
return done;
}


int GetAttach64(void)
{ int done=0;

/* called with buffer full of initial line of data */

while (!done)
	{
	if (strlen(trim(buffer))==0)
		{
		/* Done with octets */
		/* close attach file */
		if (strlen(residue64)>0)
			{
			DecodeOctets();
			}
		fclose(outatt);
		AddAttachment(outattname,attname);
		free(outattname);
		done=1;
		}
	else if (DecodeOctets()==2)
			{
			/* reached end of octets */
			/* Close attfile and add to header file */
			fclose(outatt);
			AddAttachment(outattname,attname);
			free(outattname);
			attname[0]=0;
			/* Set done to one to indicate success */
			done=1;
			}
	fgets(buffer,sizeof(buffer),in);
	}
return 1;
}
