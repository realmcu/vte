###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE tests.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
###################################################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE tests.
#
###################################################################################################
#
#   Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
#   THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
#   BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
#   Freescale Semiconductor, Inc.
#     
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Francois GAFFIE/RB657C       01/08/2004     TLSbo40930   VTE 1.3 integration
#Ludovic DELASPRE/RC149C      16/09/2004     TLSbo42022   VTE 1.5 integration
#Ludovic DELASPRE/RC149C      18/10/2004     TLSbo43861   update with new API
#Ludovic DELASPRE/RC149C      02/12/2004     TLSbo45299   CHUCK drop2.5 W449 test campaign
#Igor SEMENCHUKOV/SMNG001C    03/12/2004     TLSbo43523   Added WB AMR codec tests
#Dmitriy KAZACHKOV/e1403c     06/12/2004     TLSbo43522   Added G726 codec tests
#Ludovic DELASPRE/RC149C      20/01/2005     TLSbo45643   CTE 1.7 integration
#Irina INKINA/RC149C          15/02/2005     TLSbo47117   Added G.723.1 codec tests
#Dmitry SIMAKOV/smkd001c      15/02/2005     TLSbo47117   Added MP3 and jpeg codec tests
#Dmitry SIMAKOV/smkd001c      24/02/2005     TLSbo47117   Added PNG tests
#Dmitry SIMAKOV/smkd001c      03/03/2005     TLSbo47117   Added WBMP and BMP codecs 
#L.Delaspre\rc149c            15/03/2005     TLSbo47726   VTE 1.8 integration
#L.Delaspre\rc149c            16/03/2005     TLSbo48584   ARM11 codecs P1 tests
#L.Delaspre\rc149c            29/03/2005     TLSbo49055   ARM11 codecs P2 tests
#B.Giabicani\r70160c	      18/04/2005     TLSbo49674   ARM11 codecs P3 tests
#S. V-Guilhou\svan01c	      19/05/2005     TLSbo50534   ARM11 codecs P4 tests
#D.Simakov/smkd001c           27/05/2005     TLSbo47117   Added WMV9 decoder tests
#D.Simakov/smkd001c           31/05/2005     TLSbo47117   Added midi parser testapp
#S. V-Guilhou/svan01c	      07/06/2005     TLSbo50938   ARM11 codecs P4 tests: Midi Parser & WMA
#S. V-Guilhou/svan01c	      24/06/2005     TLSbo51902   ARM11 codecs P5 tests: Real Audio & Video
#D.Simakov/smkd001c           10/08/2005     TLSbo53249   ARM11 codecs P6 tests: MPEG4 AAC encoder
#D.Simakov/smkd001c           25/08/2005     TLSbo53252   ARM11 codecs P6 tests: H.264/AVC encoder
#D.Simakov / smkd001c         06/09/2005     TLSbo53247   AAC+ decoder
#S. V-Guilhou/svan01c	      09/09/2005     TLSbo54090   ARM11 codecs P6 tests
#D.Simakov/smkd001c           14/10/2005     TLSbo57009   Update
#S. V-Guilhou/svan01c	      23/11/2005     TLSbo57360   Replace AACLC_ by AACD_
#D. Simakov                   17/02/2005     TLSbo61033   Update 
###################################################################################################
#Portability: Indicate if this module is portable to other compilers or platforms. 
#             If not, indicate specific reasons why is it not portable.
#//
###################################################################################################


CODECS_DIR     = /local/linux_baseport/REF/armcodecs/ARM11
MMAPI_HDIR     = $(CODECS_DIR)/API_include

AACD_LIBDIR   = $(CODECS_DIR)/src/Audio/aac_dec/library
AACD_HDIR     = $(CODECS_DIR)/src/Audio/aac_dec/library/include
AACD_INPUT    = $(CODECS_DIR)/src/Audio/aac_dec/test/input
AACD_REF      = $(CODECS_DIR)/src/Audio/aac_dec/test/ref
AACD_LIBRARY   = aac_dec_arm11_ELINUX

H264D_LIBDIR    = $(CODECS_DIR)/src/Video/H264_dec/library
H264D_HDIR      = $(CODECS_DIR)/src/Video/H264_dec/library/include
H264D_INPUT     = $(CODECS_DIR)/src/Video/H264_dec/test/input
H264D_REF       = $(CODECS_DIR)/src/Video/H264_dec/test/ref
H264D_LIBRARY   = h264_dec_ipu_arm11_ELINUX

H264E_LIBDIR    = $(CODECS_DIR)/src/Video/H264_enc/library
H264E_HDIR      = $(CODECS_DIR)/src/Video/H264_enc/library/include
H264E_INPUT     = $(CODECS_DIR)/src/Video/H264_enc/test/input
H264E_REF       = $(CODECS_DIR)/src/Video/H264_enc/test/ref
H264E_LIBRARY   = h264_enc_arm11_ELINUX


NBAMRE_LIBDIR  = $(CODECS_DIR)/src/speech/nb_amr/library
NBAMRE_INPUT   = $(CODECS_DIR)/src/speech/nb_amr/test_vectors
NBAMRE_REF     = $(CODECS_DIR)/src/speech/nb_amr/test_vectors
NBAMRE_LIBRARY = nbamr_enc_arm11_ELINUX

NBAMRD_LIBDIR  = $(CODECS_DIR)/src/speech/nb_amr/library
NBAMRD_INPUT   = $(CODECS_DIR)/src/speech/nb_amr/test_vectors
NBAMRD_REF     = $(CODECS_DIR)/src/speech/nb_amr/test_vectors
NBAMRD_LIBRARY = nbamr_dec_arm11_ELINUX



WBAMRE_LIBDIR   = $(CODECS_DIR)/src/speech/wb_amr/library
WBAMRE_HDIR     = $(CODECS_DIR)/src/speech/wb_amr/library/include
WBAMRE_INPUT    = $(CODECS_DIR)/src/speech/wb_amr/test_vectors
WBAMRE_REF      = $(CODECS_DIR)/src/speech/wb_amr/test_vectors
WBAMRE_LIBRARY  = wbamr_enc_arm11_ELINUX

WBAMRD_LIBDIR   = $(CODECS_DIR)/src/speech/wb_amr/library
WBAMRD_HDIR     = $(CODECS_DIR)/src/speech/wb_amr/library/include
WBAMRD_INPUT    = $(CODECS_DIR)/src/speech/wb_amr/test_vectors
WBAMRD_REF      = $(CODECS_DIR)/src/speech/wb_amr/test_vectors
WBAMRD_LIBRARY  = wbamr_dec_arm11_ELINUX


MPEG4E_LIBDIR  = $(CODECS_DIR)/src/Video/MPEG4_enc/library
MPEG4E_HDIR    = $(CODECS_DIR)/src/Video/MPEG4_enc/library/include
MPEG4E_INPUT   = $(CODECS_DIR)/src/Video/MPEG4_enc/test/input
MPEG4E_REF     = $(CODECS_DIR)/src/Video/MPEG4_enc/test/ref
MPEG4E_LIBRARY = mpeg4_enc_arm11_ELINUX

MPEG4D_LIBDIR  = $(CODECS_DIR)/src/Video/MPEG4_dec/library
MPEG4D_HDIR    = $(CODECS_DIR)/src/Video/MPEG4_dec/library/include
MPEG4D_INPUT   = $(CODECS_DIR)/src/Video/MPEG4_dec/test/input
MPEG4D_REF     = $(CODECS_DIR)/src/Video/MPEG4_dec/test/ref
MPEG4D_LIBRARY = mpeg4_dec_arm11_ELINUX


G723D_LIBDIR   = $(CODECS_DIR)/src/speech/g.723.1/library
G723D_HDIR     = $(CODECS_DIR)/src/speech/g.723.1/library/include
G723D_INPUT    = $(CODECS_DIR)/src/speech/g.723.1/test_vectors
G723D_REF      = $(CODECS_DIR)/src/speech/g.723.1/test_vectors
G723D_LIBRARY  = g723_dec_arm11_ELINUX

G723E_LIBDIR   = $(CODECS_DIR)/src/speech/g.723.1/library
G723E_HDIR     = $(CODECS_DIR)/src/speech/g.723.1/library
G723E_INPUT    = $(CODECS_DIR)/src/speech/g.723.1/test_vectors
G723E_REF      = $(CODECS_DIR)/src/speech/g.723.1/test_vectors
G723E_LIBRARY  = g723_enc_arm11_ELINUX


JPEGE_HDIR     = $(CODECS_DIR)/src/image/jpeg_enc/library/include
JPEGE_LIBDIR   = $(CODECS_DIR)/src/image/jpeg_enc/library
JPEGE_INPUT    = $(CODECS_DIR)/src/image/jpeg_enc/test/input
JPEGE_REF      = $(CODECS_DIR)/src/image/jpeg_enc/test/ref
JPEGE_LIBRARY  = jpeg_enc_arm11_ELINUX

JPEGD_HDIR     = $(CODECS_DIR)/src/image/jpeg_dec/library/include
JPEGD_LIBDIR   = $(CODECS_DIR)/src/image/jpeg_dec/library
JPEGD_INPUT    = $(CODECS_DIR)/src/image/jpeg_dec/test/input
JPEGD_REF      = $(CODECS_DIR)/src/image/jpeg_dec/test/ref
JPEGD_LIBRARY  = jpeg_dec_arm11_ELINUX

GIFDEC_LIBDIR  = $(CODECS_DIR)/src/image/gif_dec/library
GIFDEC_HDIR    = $(CODECS_DIR)/src/image/gif_dec/library/include
GIFDEC_INPUT   = $(CODECS_DIR)/src/image/gif_dec/test/input
GIFDEC_REF     = $(CODECS_DIR)/src/image/gif_dec/test/reference
GIFDEC_LIBRARY = gif_dec_arm11_ELINUX


MP3D_LIBDIR    = $(CODECS_DIR)/src/Audio/mp3_dec/library
MP3D_HDIR      = $(CODECS_DIR)/src/Audio/mp3_dec/library/include
MP3D_INPUT     = $(CODECS_DIR)/src/Audio/mp3_dec/test/inputs
MP3D_REF       = $(CODECS_DIR)/src/Audio/mp3_dec/test/ref
MP3D_LIBRARY   = mp3_dec_arm11_ELINUX

IMY_LIBDIR     = $(CODECS_DIR)/src/speech/iMelody/library
IMY_HDIR       = $(CODECS_DIR)/src/speech/iMelody/library/include
IMY_INPUT      = $(CODECS_DIR)/src/speech/iMelody/test_vectors/input
IMY_REF        = $(CODECS_DIR)/src/speech/iMelody/test_vectors/reference
IMY_LIBRARY    = imy_conv_arm11_ELINUX



G726D_LIBDIR   = $(CODECS_DIR)/src/speech/g.726/library
G726D_HDIR     = $(CODECS_DIR)/src/speech/g.726/library/include
G726D_INPUT    = $(CODECS_DIR)/src/speech/g.726/test_vectors
G726D_REF      = $(CODECS_DIR)/src/speech/g.726/test_vectors
G726D_LIBRARY  = g726_dec_arm11_ELINUX

G726E_LIBDIR   = $(CODECS_DIR)/src/speech/g.726/library
G726E_HDIR     = $(CODECS_DIR)/src/speech/g.726/library/include
G726E_INPUT    = $(CODECS_DIR)/src/speech/g.726/test_vectors
G726E_REF      = $(CODECS_DIR)/src/speech/g.726/test_vectors
G726E_LIBRARY  = g726_enc_arm11_ELINUX


PNGD_LIBDIR    = $(CODECS_DIR)/src/image/png_dec/library
PNGD_HDIR      = $(CODECS_DIR)/src/image/png_dec/library/include
PNGD_INPUT     = $(CODECS_DIR)/src/image/png_dec/test/input
PNGD_REF       = $(CODECS_DIR)/src/image/png_dec/test/references
PNGD_LIBRARY   = png_dec_arm11_ELINUX

WBMPD_LIBDIR   = $(CODECS_DIR)/src/image/wbmp_dec/library
WBMPD_HDIR     = $(CODECS_DIR)/src/image/wbmp_dec/library/include
WBMPD_INPUT    = $(CODECS_DIR)/src/image/wbmp_dec/test/input
WBMPD_REF      = $(CODECS_DIR)/src/image/wbmp_dec/test/reference
WBMPD_LIBRARY  = wbmp_dec_arm11_ELINUX


BMPD_LIBDIR    = $(CODECS_DIR)/src/image/bmp_dec/library
BMPD_HDIR      = $(CODECS_DIR)/src/image/bmp_dec/library/include
BMPD_INPUT     = $(CODECS_DIR)/src/image/bmp_dec/test/input
BMPD_REF       = $(CODECS_DIR)/src/image/bmp_dec/test/reference
BMPD_LIBRARY   = bmp_dec_arm11_ELINUX

BMPE_LIBDIR    = $(CODECS_DIR)/src/image/bmp_enc/library
BMPE_HDIR      = $(CODECS_DIR)/src/image/bmp_enc/library/include
BMPE_INPUT     = $(CODECS_DIR)/src/image/bmp_enc/test/input
BMPE_REF       = $(CODECS_DIR)/src/image/bmp_enc/test/reference
BMPE_LIBRARY   = bmp_enc_arm11_ELINUX


WMV9D_LIBDIR   = $(CODECS_DIR)/src/Video/WMV9_dec/library
WMV9D_HDIR     = $(CODECS_DIR)/src/Video/WMV9_dec/library/include
WMV9D_INPUT    = $(CODECS_DIR)/src/Video/WMV9_dec/test/input
WMV9D_REF      = $(CODECS_DIR)/src/Video/WMV9_dec/test/ref
WMV9D_LIBRARY  = wmv9_dec_arm11_ELINUX

MIDIPAR_LIBDIR  = $(CODECS_DIR)/src/speech/MIDI/library/parser
MIDIPAR_HDIR    = $(CODECS_DIR)/src/speech/MIDI/library/parser/include
MIDIPAR_INPUT   = $(CODECS_DIR)/src/speech/MIDI/test/Input
MIDIPAR_REF     = $(CODECS_DIR)/src/speech/MIDI/test/Output
MIDIPAR_LIBRARY = midi_parser_arm11_ELINUX


WMAD_LIBDIR   = $(CODECS_DIR)/src/Audio/wma_dec/library
WMAD_HDIR     = $(CODECS_DIR)/src/Audio/wma_dec/library/include
WMAD_INPUT    = $(CODECS_DIR)/src/Audio/wma_dec/test/inputs
WMAD_REF      = $(CODECS_DIR)/src/Audio/wma_dec/test/ref
WMAD_LIBRARY  = wma_dec_arm11_ELINUX



RA8D_APIDIR   = $(CODECS_DIR)/API_include
RA8D_LIBDIR   = $(CODECS_DIR)/src/Audio/realaudio_dec/library
RA8D_HDIR     = $(CODECS_DIR)/src/Audio/wma_dec/library/include
RA8D_INPUT    = $(CODECS_DIR)/src/Audio/realaudio_dec/test/input
RA8D_REF      = $(CODECS_DIR)/src/Audio/realaudio_dec/test/ref
RA8D_LIBRARY  = realaudio_dec_arm11_ELINUX


RVD_APIDIR   = $(CODECS_DIR)/API_include
RVD_LIBDIR   = $(CODECS_DIR)/src/Video/realvideo_dec/library
RVD_HDIR     = $(CODECS_DIR)/src/Video/realvideo_dec/library/include
RVD_INPUT    = $(CODECS_DIR)/src/Video/realvideo_dec/test/input
RVD_REF      = $(CODECS_DIR)/src/Video/realvideo_dec/test/ref
RVD_LIBRARY  = realvideo_dec_arm11_ELINUX


AACE_LIBDIR   = $(CODECS_DIR)/src/Audio/aac_enc/library
AACE_HDIR     = $(CODECS_DIR)/src/Audio/aac_enc/test/include
AACE_INPUT    = $(CODECS_DIR)/src/Audio/aac_enc/test/inputs
AACE_REF      = $(CODECS_DIR)/src/Audio/aac_enc/test/ref
AACE_LIBRARY  = aac_enc_arm11_ELINUX


MBAE_LIBDIR  = $(CODECS_DIR)/src/Audio/beatnik_mobilebae/library
MBAE_HDIR    = -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Engine/Client_API/Public \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/PAL_Impls/ANSI_C \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/PAL_Impls/ANSI_C/Reference \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Engine/Common \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Libraries/G711/Integration \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Libraries/G711/Library \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Libraries/zlib/Integration \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Libraries/zlib/Library \
               -I$(CODECS_DIR)/src/Audio/beatnik_mobilebae/library/Engine/Platforms/ARM/ADS
MBAE_INPUT    = $(CODECS_DIR)/src/Audio/beatnik_mobilebae/test/Common/Content_BlackBox
MBAE_LIBRARY  = midi_bae_arm11_ELINUX


J2KD_LIBDIR   = $(CODECS_DIR)/src/image/jpeg2k_dec/library
J2KD_HDIR     = $(CODECS_DIR)/src/image/jpeg2k_dec/library/include
J2KD_INPUT    = $(CODECS_DIR)/src/image/jpeg2k_dec/test/input
J2KD_REF      = $(CODECS_DIR)/src/image/jpeg2k_dec/test/reference
J2KD_LIBRARY  = jpeg2k_dec_arm11_ELINUX

J2KE_LIBDIR   = $(CODECS_DIR)/src/image/jpeg2k_enc/library
J2KE_HDIR     = $(CODECS_DIR)/src/image/jpeg2k_enc/library/include
J2KE_INPUT    = $(CODECS_DIR)/src/image/jpeg2k_enc/test/input
J2KE_REF      = $(CODECS_DIR)/src/image/jpeg2k_enc/test/reference
J2KE_LIBRARY  = jpeg2k_enc_arm11_ELINUX

AACPD_LIBDIR   = $(CODECS_DIR)/src/Audio/aacplus_dec/library
AACPD_HDIR     = $(CODECS_DIR)/src/Audio/aacplus_dec/library/MOT_aacdec/src/include
AACPD_APIDIR   = $(CODECS_DIR)/API_include
AACPD_INPUT    = $(CODECS_DIR)/src/Audio/aacplus_dec/test/testvectors
AACPD_REF      = $(CODECS_DIR)/src/Audio/aacplus_dec/test/testvectors
AACPD_LIBRARY  = aacplus_dec_arm11_ELINUX

export CODECS_DIR \
H264D_LIBDIR \
H264D_HDIR \
H264D_LIBRARY \
H264D_INPUT \
H264D_REF \
H264E_LIBDIR \
H264E_HDIR \
H264E_REF \
H264E_LIBRARY \
H264E_INPUT \
AACD_LIBRARY \
AACD_LIBDIR \
AACD_HDIR \
AACD_INPUT \
AACD_REF \
NBAMR_VAD \
NBAMRE_LIBDIR \
NBAMRE_INPUT \
NBAMRE_REF \
NBAMRE_LIBRARY \
NBAMRD_LIBDIR \
NBAMRD_INPUT \
NBAMRD_REF \
NBAMRD_LIBRARY \
WBAMRD_LIBDIR \
WBAMRD_HDIR \
WBAMRD_INPUT \
WBAMRD_REF \
WBAMRD_LIBRARY \
WBAMRE_LIBDIR \
WBAMRE_HDIR \
WBAMRE_INPUT \
WBAMRE_REF \
WBAMRE_LIBRARY \
MPEG4E_LIBDIR \
MPEG4E_HDIR \
MPEG4E_INPUT \
MPEG4E_REF \
MPEG4E_LIBRARY \
MPEG4D_LIBDIR \
MPEG4D_HDIR \
MPEG4D_INPUT \
MPEG4D_REF \
MPEG4D_LIBRARY \
MMAPI_HDIR \
G723D_LIBDIR \
G723D_HDIR \
G723D_INPUT \
G723D_REF \
G723D_LIBRARY \
G723E_LIBDIR \
G723E_HDIR \
G723E_INPUT \
G723E_REF \
G723E_LIBRARY \
MP3D_LIBDIR \
MP3D_HDIR \
MP3D_INPUT \
MP3D_REF \
MP3D_LIBRARY \
JPEGE_LIBDIR \
JPEGE_HDIR \
JPEGE_INPUT \
JPEGE_REF \
JPEGE_LIBRARY \
JPEGD_LIBDIR \
JPEGD_HDIR \
JPEGD_INPUT \
JPEGD_REF \
JPEGD_LIBRARY \
IMY_LIBDIR \
IMY_HDIR \
IMY_LIBRARY \
IMY_INPUT \
IMY_REF \
G726D_LIBDIR \
G726E_LIBDIR \
G726D_LIBRARY \
G726E_LIBRARY \
G726D_HDIR \
G726E_HDIR \
G726D_INPUT \
G726E_INPUT \
G726D_REF \
G726E_REF \
GIFDEC_LIBDIR \
GIFDEC_LIBRARY \
GIFDEC_INPUT \
GIFDEC_REF \
GIFDEC_HDIR \
PNGD_LIBDIR \
PNGD_LIBRARY \
PNGD_INPUT \
PNGD_REF \
PNGD_HDIR \
WBMPD_LIBDIR \
WBMPD_HDIR \
WBMPD_INPUT \
WBMPD_REF \
WBMPD_LIBRARY \
BMPD_LIBDIR \
BMPD_HDIR \
BMPD_INPUT \
BMPD_REF \
BMPD_LIBRARY \
BMPE_LIBDIR \
BMPE_HDIR \
BMPE_INPUT \
BMPE_REF \
BMPE_LIBRARY \
WMV9D_LIBDIR \
WMV9D_HDIR  \
WMV9D_INPUT  \
WMV9D_REF  \
WMV9D_LIBRARY \
MIDIPAR_LIBDIR \
MIDIPAR_HDIR \
MIDIPAR_INPUT \
MIDIPAR_REF \
MIDIPAR_LIBRARY \
WMAD_LIBDIR \
WMAD_HDIR  \
WMAD_INPUT  \
WMAD_REF  \
WMAD_LIBRARY \
RA8D_APIDIR \
RA8D_LIBDIR \
RA8D_HDIR \
RA8D_INPUT \
RA8D_REF \
RA8D_LIBRARY \
RVD_APIDIR \
RVD_LIBDIR \
RVD_HDIR \
RVD_INPUT \
RVD_REF \
RVD_LIBRARY \
AACE_LIBDIR \
AACE_HDIR \
AACE_INPUT \
AACE_REF \
AACE_LIBRARY \
MBAE_LIBDIR \
MBAE_HDIR \
MBAE_INPUT \
MBAE_LIBRARY \
J2KD_LIBDIR \
J2KD_HDIR \
J2KD_INPUT \
J2KD_REF \
J2KD_LIBRARY \
J2KE_LIBDIR \
J2KE_HDIR \
J2KE_INPUT \
J2KE_REF \
J2KE_LIBRARY \
AACPD_LIBDIR \
AACPD_HDIR \
AACPD_APIDIR \
AACPD_INPUT \
AACPD_REF \
AACPD_LIBRARY \

SUBDIRS += aacplus_codec_tests 
SUBDIRS += jpeg2k_codec_tests 
#SUBDIRS += beatnik_midi_tests 
SUBDIRS += h264_decoder_tests 
SUBDIRS += aac_codec_tests/aaclc_decoder_testapp
SUBDIRS += realvideo_codec_tests 
SUBDIRS += realaudio8_codec_tests 
SUBDIRS += midi_parser_tests 
SUBDIRS += wma_codec_tests 
SUBDIRS += wmv9_codec_tests 
SUBDIRS += bmp_codec_tests 
SUBDIRS += g723_codec_tests 
SUBDIRS += wbmp_codec_tests 
SUBDIRS += wbamr_codec_tests 
SUBDIRS += imy_converter_tests 
SUBDIRS += gif_codec_tests 
SUBDIRS += png_codec_tests 
#SUBDIRS += still_pictures_tests 
#SUBDIRS += amr_codec_tests 
SUBDIRS += mp3_codec_tests 
SUBDIRS += jpeg_codec_tests 
SUBDIRS += mpeg4_codec_tests 
SUBDIRS += nbamr_codec_tests 
SUBDIRS += g726_codec_test 

all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done

