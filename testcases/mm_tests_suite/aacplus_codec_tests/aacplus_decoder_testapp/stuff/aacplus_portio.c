/*******************************************************************************
*
*   CHANGE HISTORY
*   dd/mm/yy   Code Ver     Description                        Author
*   --------   -------      -----------                        ------
*   08/03/2000  0.01        Added this template                Nanda
*                                                              Kishore A.S
*   08/03/2000  0.02        Integration changes                Nanda
*                                                              Kishore A.S
*   14/04/2000  0.03        Clean up, error codes              Nanda
*                                                              Kishore A.S
*   03/05/2000  0.04        Memory design changes              B.Venkatarao
*   18/07/2003    05        New window-ola design              B.Venkatarao
*   16/04/2004    06        Relocatability changes             Vishala
*   08/07/2004    07        Modified proto-type of
*                           AACD_writeout                      S.Nachiappan
*   08/07/2004    08        Output-file suffix is set          S.Nachiappan
*                           to ".hex" if TARGET_FILEIO is
*                           defined. (for ARM ADS)
*   23/07/2004    09        Modifications for 24 bit output    Pradeep V
*   19/08/2004    10        Removed, all stream-parsing        S.Nachiappan
*                           calls, since these will be done
*                           outside of the decoder.
*   20/08/2005    11        Added code for 24 bit wav output   Ganesh Kumar C
*
****************************************************************************/

/************************* MPEG-2 NBC Audio Decoder **************************
*                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS
and edited by
Yoshiaki Oikawa (Sony Corporation),
Mitsuyuki Hatanaka (Sony Corporation)
Mike Coleman (Five Bats Research)
in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
14496-1,2 and 3. This software module is an implementation of a part of one or more
MPEG-2 NBC/MPEG-4 Audio tools as specified by the MPEG-2 NBC/MPEG-4
Audio standard. ISO/IEC  gives users of the MPEG-2 NBC/MPEG-4 Audio
standards free license to this software module or modifications thereof for use in
hardware or software products claiming conformance to the MPEG-2 NBC/MPEG-4
Audio  standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing patents.
The original developer of this software module and his/her company, the subsequent
editors and their companies, and ISO/IEC have no liability for use of this software
module or modifications thereof in an implementation. Copyright is not released for
non MPEG-2 NBC/MPEG-4 Audio conforming products.The original developer
retains full right to use the code for his/her  own purpose, assign or donate the
code to a third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice must
be included in all copies or derivative works."
Copyright(c)1996,1997.
*                                                                           *
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "aac_fix_defs.h"
#include "aac_fix_types.h"
#include "aac_fix_proto.h"
#include "aacplus_dec_interface.h"
#include <string.h>
#ifdef DUMP_CODE_REF
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#ifdef USE_AUDIOLIB
#include "au_channel.h"
#elif defined(USELIB_AF)
#include "libtsp.h"
#endif

#include "aacplus_portio.h"

#define msgout NULL

#ifdef DUMP_CODE_REF
#define outfd           pPortIo->outfd
#define fp1             pPortIo->fp1
#define PS_file_opened  pPortIo->PS_file_opened
#else
#define outfd           pPortIo->outfd
#endif

#define outFileOpend        pPortIo->outFileOpend

#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
#define aTimeDataPcm    pPortIo->aTimeDataPcm
#endif

#ifdef USE_AUDIOLIB
#define audioOutChannel pPortIo->audioOutChannel
#define outInfo         pPortIo->outInfo
#define outType         pPortIo->outType
#define err             pPortIo->err
#define writeS          pPortIo->writeS
#elif defined(USELIB_AF)
#define file_AF         pPortIo->file_AF
#endif

#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
/* brief Interleaving of output samples Interleave output samples.
* In case of mono input, copy left channel to right channel return  none
*/

void interleaveSamples(AACPD_OutputFmtType *pTimeCh0, AACPD_OutputFmtType *pTimeCh1, AACPD_OutputFmtType *pTimeOut, int frameSize, int channels)
{
        int i;
        AACPD_OutputFmtType tmp;
        
        for (i=0; i<frameSize; i++)
        {
                *pTimeOut++ = tmp = *pTimeCh0++;
                
                if(channels == 2)
                        *pTimeOut++ = *pTimeCh1++;
        }
}
#endif

/*******************************************************************************
*
*   FUNCTION NAME - write_fext
*
*   DESCRIPTION
*       This function forms appropriate file extensions depending on the
*       output channel type.
*
*   ARGUMENTS
*       cip - Channel info
*       c   - Output channel type
*       n   - Output channel number
*       ptr - Global structure pointer
*
*   RETURN VALUE
*       None.
*
******************************************************************************/
static void
write_fext(Ch_Info * cip, char c, int n, AACD_global_struct *ptr)
{
        sprintf(cip->fext, "_%c%02d", c, n);
}

/*******************************************************************************
*
*   FUNCTION NAME - AACD_open_output_files
*
*   DESCRIPTION
*       This function opens output PCM files depending on the type of output
*       channel.
*
*   ARGUMENTS
*       mip - Multichannel info
*       ptr - Pointer to global structure
*
*   RETURN VALUE
*       Success : 0
*        Error  : -1
******************************************************************************/
int
AACD_open_output_files1(sPortIO * pPortIo, AACPD_Decoder_info *dec_info, MC_Info * mip, AACD_global_struct *ptr)
{
        int             i,
                j;
        char            file[PATH_LEN];
        char            *outpath;
        
#if defined(USELIB_AF) || defined(USE_AUDIOLIB)
        outpath = dec_info->output_path;
#else
        outpath = ptr->out_ptr;
#endif
        
        outpath = pPortIo->outpath;
        
        /* following code is used for speaker configuration
        * used in wave format
        */
        j = 0;
        /* if FCenter == 1 (center speaker present)
        *   position    index   extension
        * left        1       01
        * center      0       00
        * right       2       02
        * if FCenter == 0 (center speaker absent)
        * position    index   extension
        * left        0       01
        * right       1       02
        */
        /* following line
        * for (i = ((FCenter==1) ? 0 : 1); i < FChans; i++)
        * changed so that in output file gets suffixed by _f00
        * instead of _f01, This is done because total number of
        * channels have been changed to 2
        */
        for (i = 0; i < FChans; i++)
        {
                write_fext(&mip->ch_info[j], 'f', i, ptr);
                j++;
        }
        for (i = 0; i < SChans; i++)
        {
                write_fext(&mip->ch_info[j], 's', i, ptr);
                j++;
        }
        /* if BFCenter == 1 (center speaker present) position    index
        * extension left       3       00 center       5       02 right 4 01
        * if BCenter == 0 (center speaker absent) position index extension
        * left     3       00 right        4       01 */
        for (i = 0; i < BChans; i++)
        {
                write_fext(&mip->ch_info[j], 'b', i, ptr);
                j++;
        }
        for (i = 0; i < LChans; i++)
        {
                write_fext(&mip->ch_info[j], 'l', i, ptr);
                j++;
        }
#if defined(USELIB_AF) || defined(USE_AUDIOLIB)
        /* open file */
        if (outFileOpend != 1)
        {
                strcpy (file, outpath);
#ifdef USELIB_AF
#ifdef OUTPUT_24BITS
                file_AF = AFopenWrite(file, (256*2)+6, 2 /*dec_info->aacpd_num_channels*/, dec_info->aacpd_sampling_frequency, msgout);
#else
                file_AF = AFopenWrite(file, (256*2)+5, 2 /*dec_info->aacpd_num_channels*/, dec_info->aacpd_sampling_frequency, msgout);
#endif
                if(!file_AF)
                {
                        printf("AFopnWrite Error! \n");
                        return (-1);
                }
                else
                {
                        outFileOpend = 1;
                }
#else
                outType = TYPE_AUTODETECT;
                outInfo.fpScaleFactor = 1.0f;
                outInfo.sampleRate = dec_info->aacpd_sampling_frequency;
                outInfo.nChannels = 2;
                outInfo.valid = 1;
                outInfo.bitsPerSample = 16;
                outInfo.nSamples = 0;
                outInfo.isLittleEndian = 1;
                
                err =  AuChannelOpen(&audioOutChannel,
                        file,
                        AU_CHAN_WRITE,
                        &outType,
                        &outInfo);
                if ( err )
                {
                        fprintf( stderr, "\n AuChannelOpen(AU_CHAN_WRITE) failed\n");
                }
                if (err == AU_CHAN_OPEN_FAILED)
                {
                        return (-1);
                }
                else
                {
                        outFileOpend = 1;
                }
#endif
        }
#else /* either PCM or HEX */
        for (i = 0; i < Chans; i++)
        {
                if (!(mip->ch_info[i].present)
                        || (mip->ch_info[i].file_opened == 1))
                {
                        continue;
                }
                strcpy (file, outpath);
                strcat(file, mip->ch_info[i].fext);
#if defined(DUMP_CODE_REF)
                strcat(file, ".pcm");
                outfd[i] = creat(file, 0666);
                if (outfd[i] < 0)
                {
                        return(-1);
                }
                else
                {
                        mip->ch_info[i].file_opened = 1;
                }
#else
                strcat(file, ".hex");
                /* open file for hex data */
                outfd[i] = fopen(file, "w");
                if (outfd[i] == NULL)
                {
                        return(-1);
                }
                else
                {
                        mip->ch_info[i].file_opened = 1;
                }
#endif
        }
#endif
        return(0);
}

#ifdef DUMP_CODE_REF
int PS_open_output_file1(sPortIO * pPortIo, AACD_global_struct *ptr)
{
        char *outpath;
        char file[PATH_LEN];
        short data;
        close(outfd[0]);
        outpath = ptr->out_ptr;
        
        strcpy (file, outpath);
        strcat(file, "_f01");
        strcat(file, ".pcm");
        outfd[1] = creat(file, 0666);
        if (outfd[1] < 0)
        {
                return(-1);
        }
        /* copy the content of first file in second file */
        
        strcpy (file, outpath);
        strcat(file, "_f00");
        strcat(file, ".pcm");
        fp1 = fopen(file, "rb");
        if (fp1 == NULL)
        {
                return(-1);
        }
        while(!feof(fp1))
        {
                if(fread(&data, sizeof(short), 1, fp1) == 1)
                {
                        write(outfd[1], &data, 2);
                }
        };
        fclose(fp1);
        
        outfd[0] = open(file, O_WRONLY|O_APPEND);
        if (outfd[0] < 0)
        {
                return(-1);
        }
        return 0;
}
#endif

/*******************************************************************************
*
*   FUNCTION NAME - AACD_writeout
*
*   DESCRIPTION
*       This function is used to write the output data to a file. 16 or 32 bit
*       output is written.
*
*   ARGUMENTS
*       data - Pointer to the output data for all channels
*       mip  - Multichannel info
*       ptr  - Pointer to global structure
*
*   RETURN VALUE
*       Success : 0
*        Error  : -1
******************************************************************************/
int AACD_writeout1(sPortIO * pPortIo, AACPD_Decoder_info *dec_info,
                   AACPD_OutputFmtType data[][AACP_FRAME_SIZE],
                   MC_Info * mip,
                   AACD_global_struct *ptr, int* ShouldOpen,
                   int* ShouldClose)
{
        int             i;
        int             size;
        int                num_chans;
        
#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
        int             j;
        int localVar[Chans];
#endif     
#ifdef USELIB_AF
        AACPD_OutputFmtType pcmData;
        short int  nNumWrite;
        float fl[1];
#endif
        if (AACD_open_output_files1(pPortIo, dec_info, mip, ptr)!=0)
                return(-1);
        
        num_chans = Chans;
        
        if (*ShouldClose == 1)
        {
#ifdef USE_AUDIOLIB
                AuChannelClose(audioOutChannel);
#elif defined(USELIB_AF)
                AFclose(file_AF);
#else
                if (PS_file_opened != 1)
                {
                        for (i=0; i < num_chans; i++)
                        {
                                if (mip->ch_info[i].present)
                                {
#ifdef DUMP_CODE_REF
                                        close(outfd[i]);
#else
                                        fclose(outfd[i]);
#endif
                                }
                        }
                }
                else
                {
                        close (outfd[0]);
                        close (outfd[1]);
                }
#endif
                *ShouldClose = 0;
                return(0);
        }
#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
        if (dec_info->aacpd_num_channels == 1)
        {
                interleaveSamples(&data[0][0], &data[0][0], aTimeDataPcm, dec_info->aacpd_len, 2);
        }
        else if(dec_info->aacpd_len != AACP_FRAME_SIZE)
        {
        /* this is possible for AAC LC test vectors
        * This check is done so that parametric stereo
        * data can be written to file properly
        * in case of aac vectors stereo o/p will be present in
        * data[0] and data[1] whereas in PS it will be in
        * data[0] and data[1]
                */
                j = 0;
                for (i=0; i < num_chans; i++)
                {
                        if (!(mip->ch_info[i].present))
                        {
                                continue;
                        }
                        else
                        {
                                localVar[j] = i;
                                j++;
                        }
                }
                interleaveSamples(&data[localVar[0]][0], &data[localVar[1]][0], aTimeDataPcm, dec_info->aacpd_len, 2);
        }
        else
        {
                /* parametric stero case as SBR currently supports only mono */
                interleaveSamples(&data[0][0], &data[1][0], aTimeDataPcm, dec_info->aacpd_len, 2);
        }
#ifdef USE_AUDIOLIB
        err = AuChannelWriteShort(audioOutChannel,&aTimeDataPcm[0],(2*dec_info->aacpd_len),&writeS);
#else
        for(nNumWrite = 0; nNumWrite < 2*dec_info->aacpd_len; nNumWrite++)
        {
                pcmData = aTimeDataPcm[nNumWrite];
#ifdef OUTPUT_24BITS
                fl[0] = (float)pcmData/256.;
#else
                fl[0] = (float)pcmData;
#endif
                AFfWriteData(file_AF,fl,1);
        }
#endif
#elif defined(DUMP_CODE_REF) /* PCM */
        if ((dec_info->sbrd_ps_present != 1) && (PS_file_opened != 1)) 
        {
                /* this is true for SBR as well as AAC LC decoder */
                size = dec_info->aacpd_len * sizeof(short);
                for (i = 0; i < num_chans; i++)
                {
                        if (!(mip->ch_info[i].present))
                                continue;
                        if (write(outfd[i], &data[i][0], size) != size)
                        {
                                return(-1);
                        }
                }
        }
        else
        {
                /* parametric stereo detected */
                if (PS_file_opened == 0)
                {
                /* open one more file to write right channel data and
                * copy collected left channel data to this file
                * this is done so that file sizes are same
                        */
                        if(PS_open_output_file1(pPortIo,ptr) != 0)
                        {
                                return -1;
                        }
                        else
                        {
                                PS_file_opened = 1;
                                size = dec_info->aacpd_len * sizeof(short);
                                for (i = 0; i < 2; i++)
                                {
                                        if (write(outfd[i], &data[i][0], size) != size)
                                        {
                                                return(-1);
                                        }
                                }
                        }
                }
                else
                {
                        size = dec_info->aacpd_len * sizeof(short);
                        for (i = 0; i < 2; i++)
                        {
                                if (write(outfd[i], &data[i][0], size) != size)
                                {
                                        return(-1);
                                }
                        }
                }
        }
#else /* hex o/p */
        size = dec_info->aacpd_len * sizeof(short);
        for (i = 0; i < num_chans; i++)
        {
                int j;
                if (!(mip->ch_info[i].present))
                        continue;
                for (j = 0; j < size/2; j++)
                {
                        fprintf(outfd[i], "%08x\n", (AACPD_OutputFmtType)data[i][j]);
                }
        }
#endif
        return(0);
}

