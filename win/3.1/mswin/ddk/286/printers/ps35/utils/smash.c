/***************************************************************
 * SMASH.C compress postscript files
 *
 * note: this program breaks strings with CR LF and thus
 * will produce strange stuff on postscript that contains string
 * constants (that get broken by a CR LF pair).  PS does not
 * consider LF as white space.
 *
 * NEW VERSION.  gets best compression
 *
 ***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 65

#define TRUE 1
#define FALSE 0



FILE *in, *out;
char in_buf[BUFSIZ];
char out_buf[BUFSIZ];
static int file_len = 0;
static int line_len = 0;

void output(char ch, FILE *out);
void compress(FILE *in, FILE *out);
void CRLF(void);



main (argc,argv)
int  argc;
char *argv[];
{
	int i;

	if (argc < 2) {
		fprintf(stderr, "SMASH! remove unneeded white space from postscript files.\n");
  		fprintf(stderr, "\tusage: smash <infile> [<outfile>]\n");
		fprintf(stderr, "\tif outfile not specified output goes to stdout.\n");
		fprintf(stderr, "\tif output specified word count preceds file.\n");
		exit(1);
	}

	if ((in = fopen (argv[1],"r")) == NULL) {
  		perror(argv[1]);
		exit(1);
	}
	setbuf(in, in_buf);

	if (argc <= 2) {
		out = stdout;
	} else {

		if ((out = fopen (argv[2],"wb")) == NULL) {	/* use "wb"
								 * to only use
								 * CR */
	  		perror(argv[2]);
			exit(1);
		}
		setbuf(out, out_buf);
	}

	for (i = 0; i < sizeof(int); i++) {
		fputc('x', out);	/* leave space for the count */
	}

	compress(in, out);

	CRLF();

	if (out != stdout) {
		fseek(out, 0L, SEEK_SET);
		fwrite(&file_len, sizeof(int), 1, out);
	}

	fcloseall();

	exit(0);
}


/*
 * compress a postscript file removing all comments and extra white space
 * 
 */

#define IS_WHITE(ch)	strchr(" \t\n", ch)
#define SKIP_WHITE(ch)	strchr("/{}()", ch)	/* these chars don't require
						 * white space around them */

void compress(FILE *in, FILE *out)
{
	char ch;

	ch = fgetc(in);

	while(!feof(in)) {

		if (ch == '%') {			/* comment */

			while (!feof(in) && ch != '\n')
				ch = fgetc(in);

			/* here ch == \n */

			output(ch, out);		/* comment -> WS */
		} else if (ch == '(') {
			
			while (!feof(in) && ch != ')') {
				fputc(ch, out);
				file_len++;
				line_len++;

				ch = fgetc(in);
			}

		} else {

			output(ch, out);
			ch = fgetc(in);

		}
	}
}

/*
 * output CR LF pair for binary file
 *
 * updates global file length variable
 *
 */

void CRLF()
{
	fputc(0x0D, out);
	fputc(0x0A, out);
	file_len += 2;		/* CR LF */
}


void output(char ch, FILE *out)
{
	static int was_white = FALSE;		/* was white space */
	static int needs_white = FALSE;		/* last char requires white
						 * space (ie letters) */

	if (SKIP_WHITE(ch)) {

		if ((ch == '/')  && line_len >= MAX_LINE_LEN)
        {
			CRLF();		/* white space */

			line_len = 0;
		}
		fputc(ch, out);			/* non-white */

		file_len++;

		line_len++;

		was_white = FALSE;
		needs_white = FALSE;

	} else if (IS_WHITE(ch)) {

		was_white = TRUE;

	} else {	/* is char */
	
		if (was_white && needs_white) {


			if (line_len < MAX_LINE_LEN) {
				fputc(' ', out);	/* output white */

				file_len++;

				line_len += 2;

			} else {

				CRLF();		/* white space */

				line_len = 0;
			}

			fputc(ch, out);		/* char */

			file_len++;

			was_white = FALSE;
			needs_white = TRUE;
				
		} else {
		
			fputc(ch, out);		/* char */

			file_len++;

			line_len++;
		
			was_white = FALSE;
			needs_white = TRUE;	/* require WS if next char
						 * is white followed by char */
		}
	}

}
