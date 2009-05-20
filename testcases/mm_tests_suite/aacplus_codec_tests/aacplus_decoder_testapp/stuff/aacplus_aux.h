#ifndef __AACPLUS_AUX_H__
#define __AACPLUS_AUX_H__

#include <aacplus_dec_interface.h>
#include <aac_common.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
        int            mAppAdifHeaderPresent;
        int            mAppAdtsHeaderPresent;

        char *         mpBitstreamBuf;
        int            mBitstreamBufIndex;
        int            mBitstreamCount;
        int            mInBufDone;
        int            mBytesSupplied;

        int            mBitsInHeader;

        unsigned char  mAppIbsBuf[INTERNAL_BS_BUFSIZE];
        BitstreamParam mAppBsParam;
} sBitstream;

unsigned long App_bs_look_bits (sBitstream * pBitstream, int nbits);
int App_FindFileType(sBitstream * pBitstream, int val);
int App_bs_refill_buffer (sBitstream * pBitstream);
void App_bs_readinit(sBitstream * pBitstream, char *buf, int bytes);
unsigned long App_bs_read_bits (sBitstream * pBitstream, int nbits);
int App_bs_byte_align(sBitstream * pBitstream);
int App_get_prog_config(sBitstream * pBitstream, AACPD_ProgConfig * p);
int App_get_adif_header(sBitstream * pBitstream, AACPD_Block_Params * params);
int App_get_adts_header(sBitstream * pBitstream, AACPD_Block_Params * params);

#ifdef __cplusplus
}
#endif

#endif //__AACPLUS_AUX_H__
