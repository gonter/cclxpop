#include "cclxpop.h"


char foldfile[]="inbox.fld";
char inbox[]="inbox.ccm";
char outgoingm[13]="outgoing.m";
char outgoingn[13]="outgoing.n";
char inmail[13]="incoming.mai";
char innews[13]="incoming.nws";
char *months[]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP",
		"OCT","NOV","DEC"};

int sentfolder=2,outboxfolder=1,appendhdr=0,verbose=0,news;
long timezone=0;
char *spooldir=0,*ccPath=0;
char *sigfile=0,*from=0;
