#include "comnfunc.c"

void main(void)
{  strlist *t;
addlist(0,"string1");
addlist(0,"string2");
addlist(0,"string3");
addlist(0,"string4");
t=list[0].head;
clrscr();
while(t)
	{
	printf("%s\n",t->str);
	t=t->next;
	}
}