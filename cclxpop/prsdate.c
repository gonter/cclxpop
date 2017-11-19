#include "cclxrecv.h"
#include <string.h>
#include <dos.h>
#include <stdlib.h>



void ParseDate(char *y)
{ char *g,*cnverr=0;
  struct time t;
  struct date d;
  int i;
  long t1,tzofst;

y=trim(y);
g=strtok(y," ,"); /* Check for Day of week - not used */
i=atoi(g);
if (i!=0)
	{
	d.da_day=i;
	}
else	{
	d.da_day=atoi(strtok(NULL," ,"));
	}
g=trim(strtok(NULL," ,"));
for(i=0;i<12 && strnicmp(g,months[i],3);i++);
d.da_mon=i+1;
d.da_year=atoi(strtok(NULL," ,"));
t.ti_hour=atoi(strtok(NULL," ,:"));
t.ti_min=atoi(strtok(NULL," ,:"));
t.ti_sec=atoi(strtok(NULL," ,"));
t.ti_hund=0;
/*printf("uncorrected date/time: yr:%i Month:%i Day:%i Hour:%i Min:%i Sec:%i\n",
	d.da_year,d.da_mon,d.da_day,t.ti_hour, t.ti_min,t.ti_sec);*/


g=strtok(NULL," ,");
tzofst=strtol(g,&cnverr,10);
if (!*cnverr)  /* Conversion string ended with a 0 */
	{
/* take example of message is +0200, timezone is -0400 */
/* in that case we must subtract 6 hours from the incoming msg time */
/* to put the message time in the correct format */
	tzofst-=(timezone);
	t1=dostounix(&d, &t)-tzofst*36;
	unixtodos(t1, &d, &t);
/* now update msghdr1*/
	}
if (verbose==1)
printf("Corrected date/time: yr:%i Month:%i Day:%i Hour:%i Min:%i Sec:%i\n",
	d.da_year,d.da_mon,d.da_day,t.ti_hour, t.ti_min,t.ti_sec);

msghdr1.year=d.da_year;
msghdr1.month=d.da_mon;
msghdr1.day=d.da_day;
msghdr1.hour=t.ti_hour;
msghdr1.min=t.ti_min;
msghdr1.sec=t.ti_sec;
msghdr1.hund=0;
}
