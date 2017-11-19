#ifndef _CCMAIL_

#define _CCMAIL_

#include <stdio.h>

#define CCTO   0x61
#define CCFROM 0x22
#define CCCC   0x0A
#define CCBCC  0x09
/* Not a valid ccMail type, used for Newsgroups */
#define CCNEWS 0x44

#define BUFSIZE 1000
#define HDRSTART	0
#define HDREND		7


/* Lists: From, To, cc, bcc, newsgroups */
#define NUMLIST 5

/* number of the folder to which sent mail is routed*/
extern int sentfolder,outboxfolder,appendhdr,verbose;
extern char *spooldir,*from,*sigfile;
extern char *ccPath;
extern char foldfile[],inbox[],outgoingm[],outgoingn[],inmail[],innews[];
extern char *months[];
extern long timezone;

typedef struct def_ccm {
int folder;
unsigned int unk1,unk2;
long unk3,fileref,unk4;
char day,month;
int year;
char hund,sec,min,hour;
} ccm;

typedef struct def_msghdr{
long j1,j2;
char day,month;
int year;
char hund,sec,min,hour;
int header,attach;
long size;
} msghdr;

typedef struct def_adrec{
long type;
long len1;
char k[24];
long set1a,j2,j3;
char settoone,j4,j5;
int  length;
} adrec;

struct subj_def {
int t;
char d[9];
int length;
};

typedef struct def_strlist
	{
	struct def_strlist *next;
	char	*str;
	} strlist;


typedef struct def_strlistarray
	{
	strlist *head,*tail;
	} deflist;


extern int InAddrBook(char *name, char *resolved);
extern void resolve_address(FILE *outfile, char *name, int *first);
extern int GenerateFile(char *sourcefile);
extern int LastOutgoing(char mn);
extern void send_base64(FILE *outfile, char *fname);
extern char *trim(char *str);
extern char *getaddress(char *str);
extern char *strustr(char *s1, char *s2);
extern void addlist(int i, char *str);
extern void destroylist(int i);
#endif