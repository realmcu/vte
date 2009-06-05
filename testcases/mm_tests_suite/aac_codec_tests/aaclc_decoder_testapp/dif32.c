#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#define     BITS_MAX        32
#define     HEX             0
#define     DEC             1
#define     FRAC            2
#define     EXP             3

void    usage (char *name);
void    cmp_init (int argc, char *argv[]);
void update_stats (double fval1, double fval2);
int get_inp (FILE *fp, int type, double *fval);
int data_type (FILE * fp);

/* Params */
FILE   *fp[2];
int     type[2];
int     bits, MAX1 = 0;
long    delta, wait;

/* Statistics */
int     bits_acc, delta_sign;
double  mean = 0.0, mean_sq = 0, diff_max = 0.0, fdelta, fval_max = 0;
double  perc_err = 0.0, max_perc_err = 0.0;
long    diff[BITS_MAX + 2], others = 0;

int
main_cmp (int argc, char *argv[])
{
    long    values = 0;
    double  fval1, fval2, tmpf;
    long    i_mean, i_rms, i, print = 1;
    long    tmp, max = 0;
    int av0, av1;
    double  f_rms;

    if ((argc < 3) || (argc > 4))
        usage (argv[0]);

    cmp_init (argc, argv);

    av0 = get_inp (fp[0], type[0], &fval1);
    av1 = get_inp (fp[1], type[1], &fval2);

    while (av0 & av1)
    {
        update_stats (fval1, fval2);
        values++;
        if ((delta > MAX1) && (print))
        {
            others++;
        }
        else
        {
            if (delta <= MAX1)
            {
                if (fdelta > 0.0)
                {
                    tmpf = (fdelta > 0.0) ? log (fdelta) / log (2.0) : 0.0;
                    tmpf = (tmpf < -1.0) ? -1.0 : tmpf;
                }
                else
                    tmpf = -1.0;
                i = (int) (tmpf + 1.0);
                max = (i > max) ? i : max;
                diff[i]++;
            }
            else if (!print)
                others++;
        }
        av0 = get_inp (fp[0], type[0], &fval1);
        av1 = get_inp (fp[1], type[1], &fval2);
    }
    if (values > 0)
	{
        i_mean = (long) (65536.0 * mean / values);
        i_rms = (long) (65536.0 * pow (mean_sq / values, 0.5));
        f_rms =  (pow (mean_sq / values, 0.5));
	}
    else
	{
        i_mean = 0;
        i_rms = 0;
	}
    bits_acc = BITS_MAX;
    i = 1;

    if (diff_max > 0)
        bits_acc = BITS_MAX - 1 - ((int) (log (diff_max) / log (2.0)));
    else
        bits_acc = BITS_MAX;

    i = (long) (diff_max);
    tmp = (long) ((diff_max - (double) i) * 65536);
    
    fclose(fp[0]);
    fclose(fp[1]);
    return (values-diff[0]);
}

void
update_stats (double fval1, double fval2)
{
    double max_tmp;
    max_tmp = (fabs (fval1) > fabs (fval2)) ? fabs (fval1) : fabs (fval2);
    fval_max = (fval_max < max_tmp) ? max_tmp : fval_max;
    fdelta = fabs (fval1 - fval2);
	delta_sign = (fval1 >= fval2) ? 1 : 0;
    if (max_tmp > 0.0)
        perc_err = fdelta / max_tmp;
    else
        perc_err = 0.0;
    max_perc_err = (max_perc_err < perc_err) ? perc_err : max_perc_err;
    delta = (long) (fdelta + 0.5);
    mean_sq += (fdelta*fdelta);
    mean += (fval2 - fval1);
    diff_max = (diff_max < fdelta) ? fdelta : diff_max;

    return;
}

int
get_inp (FILE *fp, int type, double *fval)
{
    long int val;
    double tmp;

    switch (type)
    {
        case HEX:
            fscanf (fp, "%lx", &val);
            /* val = (val > 0x7fff) ? (val - 0x10000) : val; */
            *fval = (double) val;
            break;
        case DEC:
            fscanf (fp, "%ld", &val);
            *fval = (double) val;
            break;
        case FRAC:
            fscanf (fp, "%lg", &tmp);
            *fval = tmp;
            break;
        case EXP:
            fscanf (fp, "%le", &tmp);
            *fval = tmp;
            break;
    }
    if (feof(fp))
        return 0;
    else
        return 1;
}

void
cmp_init (int argc, char *argv[])
{
        

    int     i;
    char    aa[20];
    
    for (i = 0; i < 2; i++)
    {
        if ((fp[i] = fopen (argv[i + 1], "r")) == NULL)
        {
            assert(!"cant open file");
        }
        type[i] = data_type (fp[i]);
        rewind (fp[i]);
    }
    if ((type[0] == HEX) && (type[1] == DEC))
        type[1] = HEX;
    if ((type[0] == DEC) && (type[1] == HEX))
        type[0] = HEX;
    for (i = 0; i < 2; i++)
    {
        switch (type[i])
        {
            case HEX:
                sprintf (aa, "Hexadecimal");
                break;
            case DEC:
                sprintf (aa, "Decimal");
                break;
            case FRAC:
                sprintf (aa, "Fractional");
                break;
            default:
                sprintf (aa, "Unknown");
                break;
        }
        //printf ("\n File \"%s\" is in %s format", argv[i + 1], aa);
    }
    //printf ("\n");

    if (argc == 3)
        bits = 0;
    else
        sscanf (argv[3], "%d", &bits);

    if (bits > BITS_MAX)
    {
        //printf ("\n Warning : \"bits = %d\" is too large. A value of %d assumed.\n\n",
        //    bits, BITS_MAX);
        bits = BITS_MAX;
    }

    //printf ("______________________________ Start ___________________________________\n\n");
    //printf ("Comparing files \"%s\" & \"%s\" for differences in values....\n",
    //    argv[1], argv[2]);

    MAX1 = (1 << bits) - 1;

    for (i = 0; i <= BITS_MAX + 1; i++)
        diff[i] = 0;

    if ((type[0] * type[0] + type[1] * type[1]) > 3)
        wait = 5000;
    else
        wait = 50000;

    return;
}

int
data_type (FILE * fp)
{
    int     j;
    char    a;

    j = DEC;
    while (!feof (fp))
    {
        a = toupper (getc (fp));
/*
        if (!(((a >= '0') && (a <= '9')) || ((a >= 'A') && (a <= 'F')) || (a == '-') || (a == '+')))
        {
            printf ("\nSome strange characters in your input file, check it out\n\n");
            exit (1);
        }
*/
        if (feof (fp))
            break;
        if (a == '.')
        {
            j = FRAC;
            break;
        }
        if ((a >= 'A') && (a <= 'F'))
        {
            j = HEX;
            break;
        }

    }
    return (j);
}

void
usage (char *name)
{
/*
    printf ("\n  This program finds the difference in values between successive\n \
 values in two files. The numbers can be stored in Hex, decimal\n \
 or fractional formats.\n \
 The values must be separated by atleast a blank, tab or endline.\n\n \
 USAGE > %s  file1  file2  [bits]\n\n \
 where\n \
  bits is the number of bits of error (0 to %d) to be tolerated\n \
  ^^^^     Default value of \"bits\" is zero\n \
  Format in each data file is automatically found\n\n", \
        name, BITS_MAX);
    exit (0);*/
}
