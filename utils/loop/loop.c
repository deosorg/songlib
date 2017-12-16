#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include "/usr/local/include/songlib/util.h"
#include "/usr/local/include/songlib/wave.h"
#include "/usr/local/include/songlib/rra.h"

/* change PROGRAM_NAME and PROGRAM_VERSION appropriately */

char *PROGRAM_NAME = "rraloop";
char *PROGRAM_VERSION = "0.01";

static int processOptions(int,char **);

static int dups = 10;

int
main(int argc,char **argv)
    {
    int argIndex = 1;

    int i,j,k;
    FILE *in,*out;
    RRA *h;

    argIndex = processOptions(argc,argv);

    if (argc-argIndex == 0)
	{
	in = stdin;
	out = stdout;
	}
    else if (argc-argIndex == 1)
        {
	in = OpenFile(argv[argIndex],"r");
	out = stdout;
	}
    else if (argc-argIndex == 2)
        {
	in = OpenFile(argv[argIndex],"r");
	out = OpenFile(argv[argIndex+1],"w");
	}
    else
	{
	printf("usage: rraloop [<input rra file> [<output rra file>]]\n");
	exit(-1);
	}

    h = readRRA(in,0);

    h->samples *= dups;

    writeRRAHeader(out,h,"modifiedBy: rraloop",0);

    h->samples /= dups;

    for (i = 0; i < h->samples; ++i)
        for (j = 0; j < h->channels; ++j)
	    {
	    fprintf(out,"%d\n",h->data[j][i]);
	    }

    fprintf(out,"! %d duplicates\n",dups - 1); 
    for (k = 0; k < dups - 1; ++k)
	for (i = 0; i < h->samples; ++i)
	    for (j = 0; j < h->channels; ++j)
		{
		fprintf(out,"%d\n",h->data[j][i]);
		}

    fclose(in);
    fclose(out);

    return 0;
    }

/* only -oXXX  or -o XXX options */

static int
processOptions(int argc, char **argv)
{
    int argIndex;
    int argUsed;
    int separateArg;
    char *arg;

    argIndex = 1;

    while (argIndex < argc && *argv[argIndex] == '-') {

	separateArg = 0;
	argUsed = 0;

	if (argv[argIndex][2] == '\0')
	    {
	    arg = argv[argIndex+1];
	    separateArg = 1;
	    }
	else
	    arg = argv[argIndex]+2;

	switch (argv[argIndex][1])
	    {
	    /*
	     * when option has an argument, do this
	     *
	     *     examples are -m4096 or -m 4096
	     *
	     *     case 'm':
	     *         MemorySize = atol(arg);
	     *         argUsed = 1;
	     *         break;
	     *
	     *
	     * when option does not have an argument, do this
	     *
	     *     example is -a
	     *
	     *     case 'a':
	     *	       PrintActions = 1;
	     *         break;
	     */
	    case 'v':
	        printf("%s version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
		exit(0);
		break;
	    case 'n':
	        dups = atoi(arg);
		argUsed = 1;
		break;
            default:
                Fatal("option %s not understood\n",argv[argIndex]);
            }

	if (separateArg && argUsed)
	    ++argIndex;

        ++argIndex;
        }

    return argIndex;
}
