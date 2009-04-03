###########################################################################
# Control File Format is as follows
# <ParameterName> = <ParameterValue> # Comment
#
# See ui.h for a list of supported ParameterNames
#
# The files is specified as :
#   > encoder -f <this file> -p <Param> = <Value> ...
###########################################################################

Version             = 4          # Version of the control file structure

###########################################################################
# Files
###########################################################################
SourceFilename      = "input/source.qcif/foreman.qcif"
BitFilename         = "output/bitstream.avc"     # Output compressed bitstream
TraceFilename       = "trace.log"         # Encoder trace filename
ReconFilename       = "output/foreman_recon2"         # Reconstructed video seqience
Trace               = 0         # Generate a tracefile [No=0/Yes=1]
WriteRecon          = 0         # Generate reconstructed video [0/1]
SourceFrameRate     = 30        # Source frame rate of input video

###########################################################################
# Encoder Parameters
###########################################################################
### General
Level               = 20        # Encoding level
StartFrame          = 0         # Frame number to start encoding
EndFrame            = 399       # Frame number to stop coding
EncodedFrameRate    = 15        # Frame rate of the encoded video
IPeriod             = 0         # Periodicity of I frame [0 = only once]
IntraRefreshMethod  = 0         # 0: No Intra Refresh
                                # 1: Divided into multiple regions for random 
                                #    intra refreshment. Each region assigns 
                                #    NumForcedIntra I-MB's per p-frame.
                                #    Support subqcif, qcif, qcif, cif, vga/16,
                                #    and vga/4 only
                                # 2: Insert NumForcedIntra consecutive I-MB's 
                                #    per p-frame
                                # 3: Insert NumForcedIntra I-MB per p-frame
                                #    (Support arbitrary image size)
                                # (Choose 0 for error-free channels)
NumForcedIntra      = 4         # 
IDRPeriod           = 0         # Periodicity of IDR among I frames
RDMode              = 0         # 0: bit cost LUT
                                # 1: RD optimization, mode decision 
                                #       at sub-pel level 
                                # 2: RD optimization, mode decision 
                                #       at full-pel level

###########################################################################
# Error Resilience / Slices
###########################################################################

SliceMode           = 0         # Slice mode (0=off 1=fixed #mb in slice
                                #   2=fixed #bytes in slice 3=use callback
                                #   4=FMO)
NumSliceGroups      = 1         # Number of Slice Groups, 1 == no FMO,
                                #  2 == two slice groups, etc.
SliceGroupMapType   = 0         # 0:  Interleave, 1: Dispersed,
                                # 2: Foreground with left-over, 
                                # 3: Box-out,    4: Raster Scan   5: Wipe
                                # 6: Explicit, slice_group_id read from
                                #     SliceGroupConfigFileName
SliceGroupSlicingMode = 1       # Mode to end slice of a slice grp
                                # 0 = 1 slice/slice grp;
                                # 1 = end slice basing on no of MBs/slice
                                # 2 = end slice basing on no of bits/slice
SliceGroupConfigFileName = "sgconf.cfg" # Used for slice_grp_type 0, 2, 6
SliceWidthBits      = 600       # Bits per packet threshold
SliceWidthMB        = 11        # Max num of MBs per packet

### Rate Control
TargetBitRate       = 64000     # Bits/sec. 0 - No rate control
QI                  = 34        # static quantizer parameter for I frames
QP                  = 34        # static quantizer parameter for P frames
RCMethod            = 2         # [0 - 2].
                                # 0: MSVC 
                                # 1 and 2: methods with regulation of 
                                #    buffer fullness
QualityTradeoff     = 0         # [0 - 31]. Default = 16. "0" means default
                                #           Affects MSVC only 
DelayFactor         = 3         # [0 - 15]. Default = 1. "0" means default
                                # Maximun buffer delay = 
                                #    3 * DelayFactor * source frame period 
                                ## See notes on rate control at the end
                                ## of the file
### Motion estimation
ME_Algorithm        = 1         # 0: Full search 1: FPMS 
ME_SearchRange      = 16        # +/- range of integer motion vector
NumRefFrames        = 1         # No. of reference frames to predict from

### Inter Block search modes (0 = Disable, 1 = Enable) ###
Inter16x16          = 1
Inter16x8           = 1
Inter8x16           = 1
Inter8x8            = 1
Inter8x4            = 1
Inter4x8            = 1
Inter4x4            = 1

### Bitstream format
OutputMode          = 0         # Specifies the output file type
                                # (0:Bitstream, 1:RTP, 2:IFF)

###########################################################################
# Notes
###########################################################################
# Rate Control:
#   (1) For getting high frame rate, you'll need to set a high quality
#       tradeoff factor, as well as a slightly high delay factor
#       e.g. Delay_factor = 4, Quality_tradeoff = 26
#   (2) Use the minimum delay factor value that satisfies your requirement
#   (3) Using too high a value for quality trade-off might cause buffer
#       underflow (i.e. bits generated are less than channel capacity)
###########################################################################
