
#include "\nkit64\lxapi.h"
#include "task.h"
#include <dos.h>
#include <process.h>
#include <stdlib.h>

#define IRET_CODE 0xCF

int     _Cdecl printf(const char *__format, ...);


typedef unsigned char far *VECTOR;
VECTOR far *IntVectors = (VECTOR far *)0L;

int CheckSysMgr(void)
{
   return IntVectors[0x7e] != NULL && *IntVectors[0x7e] != IRET_CODE &&
	  IntVectors[0x7f] != NULL && *IntVectors[0x7f] != IRET_CODE;
}


void keystuff(int scancode)
{  union REGS r;

r.h.ah=0x05;
r.x.cx=scancode;
int86(0x16, &r, &r); 
}



void check_ccmail(void)
{ struct task far *TCBptr;
  int i,index;

TCBptr=m_get_TCB();
for(index=-1,i=0;i<( int ) m_get_TCB_size();i++)
	if (TCBptr[i].t_hotkey==0xBF00 /*ccMail*/ ) index=i;
if (index!=-1)
	{
	if (TCBptr[index].t_state!=0 /*PS_CLOSED*/)
		{
		printf("ccMail must be closed for ccLXPOP to work properly\n");
		printf("Aborting.");
		exit(-1);
		}
	}
}