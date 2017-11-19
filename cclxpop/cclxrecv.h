#include <stdio.h>
#include "cclxpop.h"

#define countof( x ) sizeof( x )/sizeof( x[0] )

/* NUMFIELDS is number of fields definied if Fields[] array in  ccthread.c*/
#define NUMFIELDS 11

extern char buffer[1000],boundary[256],*bp;
extern char *FolderList[200],lastaddress[],outaddrname[];
extern int NumFolders,msgOk,curfolder,FolderIndex[200],news;
extern char marker[],nullstr[],*attname,*outattname;
extern char *Fields[];
/* From, To, cc, bcc, and newsgroup must be first entries to support
   use of list array*/
typedef enum def_states2 {FROM,TO,CC,BCC,NEWSGROUP,SUBJ,DATE,
	BODY,BASE64,NEEDFILENAME,BEGIN,HEADER}msgstate;
extern msgstate msg;
extern FILE *in,*out,*outbody,*outatt,*outaddr;
extern long lastfilepos, fn;
extern adrec ad;
extern msghdr msghdr1;

extern void LoadBuffer(void);
extern void ResetFilePos(void);
extern char *trimfront(char *);
extern char *trim(char *);
extern void ParseDate(char *);
extern void ParseSubj(void);
extern void HandleHeader(void);
extern int GetAttach64(void);
extern void AddAttachment(char *, char *);
extern char *getnextattname(void);
extern void WriteSubj(void);
extern void ReadFolders(void);
extern void WriteSubj(void);
extern void WriteAddrInfo(void);
extern int MakeOutgoing(void);
extern int CheckSysMgr(void);
extern void check_ccmail(void);
extern int ThreadFile(char *infile);
extern void ParseAddrInfo(int i);
extern char *trimback(char *t);
extern void SelectFolder(void);
extern void initlists(void);
