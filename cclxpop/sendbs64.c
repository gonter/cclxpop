#include <stdio.h>
#include <dos.h>


static const unsigned char Base64[]=
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void send_base64(FILE *outfile, char *fname)
/*Sends one SMTP Line worth of binary data from file f */
{ char inbuf[54],outbuf[100],*out;
  FILE *f;
  char *in;
  int t;

f=fopen(fname,"rb");
while(!feof(f))
	{
	t=fread(&inbuf,1,54,f);
	out=outbuf;in=inbuf;
	while(t>2)
		{
		*out++=Base64[(*in>>2)&63];
		*out++=Base64[((*in&3)<<4)|((in[1]>>4)&15)];
		*out++=Base64[((in[1]&15)<<2)|((in[2]>>6)&3)];
		*out++=Base64[in[2]&63];
		in+=3;t-=3;
		}
	if (feof(f))
		switch (t)
			{
			case 2:
				*out++=Base64[(*in>>2)&63];
				*out++=Base64[((*in&3)<<4)|((in[1]>>4)&15)];
				*out++=Base64[((in[1]&15)<<2)];
				*out++='=';
				break;
			case 1:
				*out++=Base64[(*in>>2)&63];
				*out++=Base64[((*in&3)<<4)];
				*out++='=';
				*out++='=';
				break;
			case 0: *out++='=';
				break;
			}
	*out++=0;
	fprintf(outfile,"%s\n",outbuf);
	}
}
