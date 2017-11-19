typedef enum def_states {START, INMSG, ENDMSG, ENDFILE} state;


long  Start[1000],Finish[1000];

int IndexFile(char *infile)
{ char fname[120], buffer[1000];
  int i=0;
  state s=START;
  FILE *f;
  long size;
strcpy(fname,spooldir);
strcat(fname,infile);
f=fopen(fname,"rt");
if (!f)
	return 0;
printf("Indexing %s\n",fname);
fseek(f,0L,2);
size=ftell(f);
fseek(f,0L,0);
ftell(f);
while(!feof(f) && i<1000)
	{
	fgets(buffer,sizeof(buffer),f);
	if (strncmp(buffer,marker,4)==0)
		{
		switch (s)
			{
			case START:
				s=INMSG;
				Start[i]=ftell(f);
				break;
			case INMSG:
				s=ENDMSG;
				Finish[i]=ftell(f)-4;
				break;
			case ENDMSG:
				s=START;
				printf("Percent Complete: %i\x0d",100L*Finish[i++]/size);
				break;
			}
		}
	}
fclose(f);
return i;
}
