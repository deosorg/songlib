#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "/usr/local/include/songlib/util.h"
#include "/usr/local/include/songlib/rra.h"

/* change PROGRAM_NAME and PROGRAM_VERSION appropriately */

char *PROGRAM_NAME = "hartley";
char *PROGRAM_VERSION = "0.01";

static int channel = 0;
static FILE *in,*out;

static void analyze(RRA *);
static double hartley(int *,int,int);
static int processOptions(int,char **);

int
main(int argc,char **argv)
    {
    int argIndex = 1;

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
        printf("usage: hartley -c<N> "
            "[<input rra file> [<output rra file>]]\n");
        exit(-1);
        }

    h = readRRA(in,0);

    analyze(h);

    fclose(in);
    fclose(out);

    return 0;
    }

static void
analyze(RRA *h)
    {
    int i;
    int *data;

    if (channel >= h->channels)
        Fatal("not enough channels (channel zero is the first channel)\n");

    fprintf(out,"RRAUDIO\n");
    fprintf(out,"createdBy: hartley\n");
    fprintf(out,"channels: 1\n");
    fprintf(out,"samples: %d\n",h->samples);
    fprintf(out,"%%%%\n");

    data = h->data[channel];

    /* Produce the yin data here.
     * The total error is the sum of the squares of the differences.
     * Divide the total error by 'scale'. Run the output through
     * normalize (see ~/songlib/trunk/utils/normalize) before
     * viewing/playing
     */

    for (i = 0; i < h->samples; ++i)
        {
        fprintf(out,"%f\n", hartley(data,h->samples,i) / h->samples);
        }
    }

static double
hartley(int *data,int size,int spot)
    {
    int i;
    double total;
    double factor;
    
    factor = 2 * 3.14159 * spot / (double) size;

    total = 0.0;
    for (i = 0; i < size; ++i)
        total += data[i] * (sin(factor * i) + cos(factor * i));

    return total;
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
             *         PrintActions = 1;
             *         break;
             */
            case 'c':
                channel = atoi(arg);
                argUsed = 1;
                break;
            case 'v':
                printf("%s version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
                exit(0);
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
