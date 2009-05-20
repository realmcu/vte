#ifndef __AACPLUS_PORTIO_H__
#define __AACPLUS_PORTIO_H__

#include <aac_fix_defs.h>
#include <aac_fix_types.h>
#include <aac_fix_proto.h>
#include <aacplus_dec_interface.h>

#ifdef USE_AUDIOLIB
#include "au_channel.h"
#elif defined(USELIB_AF)
#include "libtsp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PATH_LEN    128

typedef struct
{
#ifdef DUMP_CODE_REF
        int                 outfd[Chans];
        FILE *              fp1;
        int                 PS_file_opened;
#else
        FILE                outfd[Chans];
#endif
        unsigned char       outFileOpend;
#if defined(USE_AUDIOLIB) || defined(USELIB_AF)
        AACPD_OutputFmtType aTimeDataPcm[4* AAC_FRAME_SIZE];
#endif

#ifdef USE_AUDIOLIB
        hAudioChannel       audioOutChannel;
        AuChanInfo          outInfo;
        AuChanType          outType;
        AuChanError         err;
        int writeS;
#elif defined(USELIB_AF)
        AFILE             * file_AF;
#endif

        char                outpath[PATH_LEN];
} sPortIO;

int AACD_open_output_files1(sPortIO * pPortIo, AACPD_Decoder_info *dec_info, MC_Info * mip, AACD_global_struct *ptr);
#ifdef DUMP_CODE_REF
int PS_open_output_file1(sPortIO * pPortIo, AACD_global_struct *ptr);
#endif
int AACD_writeout1(sPortIO * pPortIo, AACPD_Decoder_info *dec_info,
                  AACPD_OutputFmtType data[][AACP_FRAME_SIZE],
                  MC_Info * mip,
                  AACD_global_struct *ptr, int* ShouldOpen,
                  int* ShouldClose);

#ifdef __cplusplus
}
#endif

#endif //__AACPLUS_PORTIO_H__
