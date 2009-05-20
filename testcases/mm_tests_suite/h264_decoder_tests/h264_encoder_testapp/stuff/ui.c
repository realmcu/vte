/*
 *
 *
 *                         Motorola Labs
 *         Multimedia Communications Research Lab (MCRL)
 *
 *              H.264/MPEG-4 Part 10 AVC Video Codec
 *
 *          This code is the property of MCRL, Motorola.
 *  (C) Copyright 2002-03, MCRL Motorola Labs. All Rrights Reserved.
 *
 *       M O T O R O L A  I N T E R N A L   U S E   O N L Y
 *
 *
 */
/*!
 *
 * \file
 *    ui.c
 *
 * \brief
 *    User Interface functions for reading encoding parameters
 *    This file supports command line and file-based input.
 *    The functions are based on the JM software model
 *
 * \author
 *    - Faisal Ishtiaq                      <faisal@motorola.com>
 *    - Raghavan (Rags) Subramaniyan        <raghavan.s@motorola.com>
 *
 * \history
 *    -Lava Kumar                           <a5912c@motorola.com>
 *    27/Dec/2004: added support for FMO
 *    - Shih-Ta Hsiang                      <hsiang@motorola.com>
 *    18/Mar/2005: imrpove support for intra MB refresh
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <global.h>
#include <AVC_VideoEncoder.h>
#include "io.h"
#include "ui.h"
#include <debug.h>

#define MAX_ITEMS_TO_PARSE  10000
#define NUM8    0
#define NUM16   1
#define NUM32   2
#define STR     3

#undef  PrintErr
#undef  Exit
#define PrintErr(s,...)
#define Exit exit(1)
//#define PrintErr
//#define Exit return 0


/*!
 ***********************************************************************
 * \brief
 *    allocates memory buf, opens file Filename in f, reads contents into
 *    buf and returns buf
 *
 * \param Filename
 *    name of control file
 ***********************************************************************
 */
char   *GetControlFileContent(char *Filename)
{
    unsigned FileSize;
    FILE   *f;
    char   *buf;

    if (NULL  (f  fopen(Filename, "r")))
    {
        PrintErr("Cannot open control file %s.\n", Filename);
        Exit;
    }

    if (0 ! fseek(f, 0, SEEK_END))
    {
        PrintErr("Cannot fseek in control file %s.\n", Filename);
        Exit;
    }

    FileSize  ftell(f);
    if (FileSize < 0 || FileSize > 60000)
    {
        PrintErr
            ("Unreasonable Filesize %d reported by ftell for control file %s.\n",
             FileSize, Filename);
        Exit;
    }
    if (0 ! fseek(f, 0, SEEK_SET))
    {
        PrintErr("Cannot fseek in control file %s.\n", Filename);
        Exit;
    }

    if ((buf  malloc(FileSize + 1))  NULL)
    {
        PrintErr("Memory Allocation Error\n");
        Exit;
    }

    // Note that ftell() gives us the file size as the file system sees it.  The actual file size,
    // as reported by fread() below will be often smaller due to CR/LF to CR conversion and/or
    // control characters after the dos EOF marker in the file.

    FileSize  fread(buf, 1, FileSize, f);
    buf[FileSize]  '\0';

    fclose(f);
    return buf;
}

/*!
 ***********************************************************************
 * \brief
 *    Returns the index number from Map[] for a given control name.
 * \param s
 *    parameter name string
 * \return
 *    the index number if the string is a valid parameter name,         \n
 *    -1 for error
 ***********************************************************************
 */
static int ParameterNameToMapIndex(char *s, ParameterMapping *MapPtr)
{
    int     i  0;

    while (MapPtr[i].TokenName ! NULL)
        if (0  strcmp(MapPtr[i].TokenName, s))
            return i;
        else
            i++;
    return -1;
};

/*!
 ***********************************************************************
 * \brief
 *    Parses the character array buf and assigns them to the input parameters
 *    in the mapping.
 * \param buf
 *    buffer to be parsed
 * \param bufsize
 *    buffer size of buffer
 ***********************************************************************
 */
void ParseContent(char *buf, int bufsize, ParameterMapping *MapPtr)
{

    char   *items[MAX_ITEMS_TO_PARSE];
    int     MapIdx;
    int     item  0;
    int     InString  0, InItem  0;
    char   *p  buf;
    char   *bufend  &buf[bufsize];
    long    IntContent;
    int     i;

    // Stage one: Generate an argc/argv-type list in items[], without comments and whitespace.
    // This is context insensitive and could be done most easily with lex(1).

    while (p < bufend)
    {
        switch (*p)
        {
            case 13:
                p++;
                break;
            case '#':                          // Found comment

                *p  '\0';                      // Replace '#' with '\0' in case of comment immediately following integer or string

                while (*p ! '\n' && p < bufend)    // Skip till EOL or EOF, whichever comes first

                    p++;
                InString  0;
                InItem  0;
                break;
            case '\n':
                InItem  0;
                InString  0;
                *p++  '\0';
                break;
            case ' ':
            case '\t':                         // Skip whitespace, leave state unchanged

                if (InString)
                    p++;
                else
                {                               // Terminate non-strings once whitespace is found

                    *p++  '\0';
                    InItem  0;
                }
                break;

            case '"':                          // Begin/End of String

                *p++  '\0';
                if (!InString)
                {
                    items[item++]  p;
                    InItem  ~InItem;
                }
                else
                    InItem  0;
                InString  ~InString;           // Toggle

                break;

            default:
                if (!InItem)
                {
                    items[item++]  p;
                    InItem  ~InItem;
                }
                p++;
        }
    }

    item--;

    for (i  0; i < item; i + 3)
    {
        if (0 > (MapIdx  ParameterNameToMapIndex(items[i], MapPtr)))
        {
            PrintErr("Parsing error in control file:\n"
                     "      Parameter Name '%s' not recognized\n",
                     items[i]);
            Exit;
        }
        if (strcmp("", items[i + 1]))
        {
            PrintErr("Parsing error in control file:\n"
                     "    '' expected as the second token in each line\n");
            Exit;
        }

        // Now interprete the Value, context sensitive...

        if (MapPtr[MapIdx].Type < NUM32)
            if (1 ! sscanf(items[i + 2], "%ld", &IntContent))
            {
                PrintErr
                    ("Parsing error: Expected numerical value for"
                     " parameter %s, found '%s'\n", items[i],
                     items[i + 2]);
                Exit;
            }

        switch (MapPtr[MapIdx].Type)
        {
            case NUM32:                        // Numerical
                *(long *) (MapPtr[MapIdx].Location)  IntContent;
                break;
            case NUM16:                        // Numerical
                *(short *) (MapPtr[MapIdx].Location)  (short) IntContent;
                break;
            case NUM8:                         // Numerical
                *(char *) (MapPtr[MapIdx].Location)  (char) IntContent;
                break;
            case STR:
                strcpy((char *) MapPtr[MapIdx].Location, items[i + 2]);
                break;
            default:
                assert("Unknown value type in the map definition");
        }
    }

}

/*!
 ***********************************************************************
 * \brief
 *   Checks the input parameters for consistency. I only check the
 *   syntax and not for semantics.
 ***********************************************************************
 */
#define Print(fmt,...)
static void CheckParameters(ParameterMapping *MapPtr)
{
    int     i  0;
    long    val  0;

    //! Verify that the encoder parameters fall within accepted ranges.
    while (MapPtr[i].TokenName ! NULL)
    {
        if (MapPtr[i].Type < NUM32)
        {
            switch (MapPtr[i].Type)
            {
                case NUM32:
                    val  *(long *) (MapPtr[i].Location);
                    break;
                case NUM16:
                    val  *(short *) (MapPtr[i].Location);
                    break;
                case NUM8:
                    val  *(char *) (MapPtr[i].Location);
                    break;
            }
            Print("> %s  %ld\n", MapPtr[i].TokenName, val);
            if (val < MapPtr[i].minVal || val > MapPtr[i].maxVal)
            {
                PrintErr
                    ("Input parameter %s  %ld exceeds its bounds [%ld %ld]\n",
                     MapPtr[i].TokenName, val, MapPtr[i].minVal,
                     MapPtr[i].maxVal);
                Exit;
            }
        }
        else
            Print("> %s  %s\n", MapPtr[i].TokenName,
                  (char *) MapPtr[i].Location);
        i++;
    }
}

static void ReadUserInput(int argc, char *argv[], ParameterMapping *MapPtr)
{
    char   *content;
    int     CLcount, ContentLen, NumberParams;

    //! Parse the command line arguments.
    CLcount  1;
    while (CLcount < argc)
    {
        //! Parse control file
        if (0  strncmp(argv[CLcount], "-f", 2))
        {
            if (CLcount  (argc - 1))
            {
                PrintErr("Control file not specified\n");
                Exit;
            }
            content  GetControlFileContent(argv[CLcount + 1]);
            Print("\nParsing Paramsfile %s\n\n", argv[CLcount + 1]);
            ParseContent(content, strlen(content), MapPtr);
            free(content);
            CLcount + 2;
        }
        else
        {
            //! Process any overrides for the params.
            if (0  strncmp(argv[CLcount], "-p", 2))
            {
                //! Collect all data until next parameter (starting with -<x>
                //! where (x is any character)), put into content, and parse it.
                CLcount++;
                ContentLen  0;
                NumberParams  CLcount;

                // determine the necessary size for content.
                // An 1000 additional bytes are allocated for spaces and \0s.
                while (NumberParams < argc && argv[NumberParams][0] ! '-')
                    ContentLen + strlen(argv[NumberParams++]);
                ContentLen + 1000;

                if ((content  malloc(ContentLen))  NULL)
                    exit(-1);                   //no_mem_exit("Configure: content");;

                content[0]  '\0';

                // concatenate all parameters itendified before
                while (CLcount < NumberParams)
                {
                    char   *source  &argv[CLcount][0];
                    char   *destin  &content[strlen(content)];

                    while (*source ! '\0')
                    {
                        //! NOTE: The parser expects a whitespace before and after a ''.
                        //! If not present then add the spaces.
                        if (*source  '')
                        {
                            *destin++  ' ';
                            *destin++  '';
                            *destin++  ' ';
                        }
                        else
                            *destin++  *source;
                        source++;
                    }
                    *destin  '\0';
                    CLcount++;
                }
                Print("<Command Line> '%s'", content);
                ParseContent(content, strlen(content), MapPtr);
                free(content);
                Print("\n");
            }
            else
            {
                PrintErr("Error in command line, argc %d, around string"
                         " '%s'\n       Missing -f or -p parameters?\n",
                         CLcount, argv[CLcount]);
                Exit;
            }
        }
    }
    Print("\nFinal Encoder Parameters\n------------------------\n");
    CheckParameters(MapPtr);
    Print("\n");
}

/*!
 ***********************************************************************
 * \brief
 *    Main function that gets user input from params file and/or
 *    command-line
 *
 * \param *p
 *    pointer to the avc structure
 *
 * \param argc
 *    number of command line parameters
 *
 * \param argv
 *    command line parameters
 ***********************************************************************
 */
void UI_GetUserInput(IOParams *o, AVC_VideoEncodeParamsStruct * e,
                     int argc, char *argv[])
{
    //! This mapping includes the parameter, its location in
    //! the structure, and the default values. NOTE: All fields in
    //! the control file must be mapped. Makes life easier.
    //! The formatting of the mapping is as follows:
    //! {<Token name>,<Variable Address>,<STRing/NUMber>,<Default NUM val>,<Default STR val>,<Min NUM>,<Max NUM>}
    //! The #defined can be found in global.h
    int     i  0;

#ifndef __ARMV6__
    ParameterMapping Map[]  {
        {"ControlFile", &o->ctlFile, STR, NA, "encoder.ctl", NA, NA},
        {"Version", &o->version, NUM16, CTL_CURRENT_VERSION, "",
         CTL_CURRENT_VERSION, CTL_CURRENT_VERSION},
        {"SourceFilename", &o->sourceFilename, STR, NA,
         "/proj/vidsrc/source.qcif/foreman.qcif", NA, NA},
        {"BitFilename", &o->bitFilename, STR, NA, "bitstream.avc", NA, NA},
        {"TraceFilename", &o->traceFilename, STR, NA, "trace.log", NA, NA},
        {"ReconFilename", &o->reconFilename, STR, NA, "recon.kev", NA, NA},
        {"Trace", &o->traceFlag, NUM8, ON, "", OFF, ON},
        {"WriteRecon", &o->writeReconstructedFlag, NUM8, ON, "", OFF, ON},
        {"SourceFrameRate", &e->sourceFrameRate, NUM16, 30, "", 1,
         30},
        {"EncodedFrameRate", &e->encodingFrameRate, NUM16, 15, "",
         1, 30},
        {"TargetBitRate", &e->targetBitRate, NUM32, 48000, "", 0,
         50000000},
        {"QualityTradeoff", &e->qualityTradeoff, NUM8, 0, "", 0,
         31},
        {"DelayFactor", &e->delayFactor, NUM8, 0, "", 0, 15},
        {"IntraRefreshMethod", &e->intraRefreshMethod, NUM8,
         DEFAULT_INTRA_REFRESH_METHOD, "", 0,
         NUM_INTRA_REFRESH_METHODS - 1},
        {"NumForcedIntra", &e->numForcedIntra, NUM8, 1, "",
         0, 4096},
        {"ME_Algorithm", &e->ME_AlgorithmID, NUM8, 0, "", 0, 2},
        {"ME_SearchRange", &e->mbIntegerSearchDistance, NUM16, 16, "",
         1, 256},
        {"Level", &e->level, NUM16, 10, "", 10, 51},
        {"StartFrame", &o->startFrameNum, NUM32, 0, "", 0, 0x7fffffff},
        {"EndFrame", &o->endFrameNum, NUM32, 400, "", 0, 0x7fffffff},
        {"IDRPeriod", &e->IDRPeriod, NUM32, 1, "", 0, 0x7fffffff},
        {"IPeriod", &e->intraPeriod, NUM32, 0, "", 0, 0x7fffffff},
        {"SliceMode", &e->sliceMode, NUM8, 0, "", 0, 4},
        {"NumRefFrames", &e->maxRefFrames, NUM8, 1, "", 1, 15},
        {"QI", &e->qi, NUM8, 31, "", -1, 51},
        {"QP", &e->qp, NUM8, 31, "", 0, 51},
        {"SliceWidthBits", &e->resyncMarkSpacing, NUM32, 400, "", 1,
         0x7fffffff},
        {"SliceWidthMB", &e->resyncMarkMBSpacing, NUM16, 12, "", 1, 30000},
        {"NumSliceGroups", &e->num_slice_groups, NUM8, 0, "", 0, 4},
        {"SliceGroupMapType", &e->slice_group_map_type, NUM8, 0, "", 0, 6},
        {"SliceGroupConfigFileName", &o->SliceGroupConfigFileName, STR, NA,
         "sg0conf.cfg", NA, NA},
        {"SliceGroupSlicingMode", &e->sliceGroupSlicingMode, NUM8, 0, "",
         0, 2},
        {"Inter16x16", &e->enableBlockSize[0], NUM8, ON, "", OFF,
         ON},
        {"Inter16x8", &e->enableBlockSize[1], NUM8, ON, "", OFF, ON},
        {"Inter8x16", &e->enableBlockSize[2], NUM8, ON, "", OFF, ON},
        {"Inter8x8", &e->enableBlockSize[3], NUM8, ON, "", OFF, ON},
        {"Inter8x4", &e->enableBlockSize[4], NUM8, ON, "", OFF, ON},
        {"Inter4x8", &e->enableBlockSize[5], NUM8, ON, "", OFF, ON},
        {"Inter4x4", &e->enableBlockSize[6], NUM8, ON, "", OFF, ON},
        {"RDMode", &e->rdMode, NUM8, 1, "", 0, 2},
        {"OutputMode", &o->outputMode, NUM8, 0, "", 0, 0},
        {"RCMethod", &e->rcMethod, NUM8, 2, "", 0, 2},
        {NULL, NULL, -1, -1, NULL, 0, 0}
    };
#else
    ParameterMapping Map[]  {
        {"ControlFile", "encoder.ctl", STR, NA, "encoder.ctl", NA, NA},
        {"Version", "CTL_CURRENT_VERSION", NUM16, CTL_CURRENT_VERSION, "",
         CTL_CURRENT_VERSION, CTL_CURRENT_VERSION},
        {"SourceFilename", "/proj/vidsrc/source.qcif/foreman.qcif", STR,
         NA,
         "/proj/vidsrc/source.qcif/foreman.qcif", NA, NA},
        {"BitFilename", "bitstream.avc", STR, NA, "bitstream.avc", NA, NA},
        {"TraceFilename", "trace.log", STR, NA, "trace.log", NA, NA},
        {"ReconFilename", "recon.kev", STR, NA, "recon.kev", NA, NA},
        {"Trace", "ON", NUM8, ON, "", OFF, ON},
        {"WriteRecon", "ON", NUM8, ON, "", OFF, ON},
        {"SourceFrameRate", "30", NUM16, 30, "", 1,
         30},
        {"EncodedFrameRate", "15", NUM16, 15, "",
         1, 30},
        {"TargetBitRate", "48000", NUM32, 48000, "", 0,
         50000000},
        {"QualityTradeoff", "0", NUM8, 0, "", 0,
         31},
        {"DelayFactor", "0", NUM8, 0, "", 0, 15},
        {"IntraRefreshMethod", "0", NUM8,
         DEFAULT_INTRA_REFRESH_METHOD, "", 0,
         NUM_INTRA_REFRESH_METHODS - 1},
        {"NumForcedIntra", "0", NUM8, 1, "",
         0, 4096},
        {"ME_Algorithm", "0", NUM8, 0, "", 0, 2},
        {"ME_SearchRange", "16", NUM16, 16, "",
         1, 256},
        {"Level", "10", NUM16, 10, "", 10, 51},
        {"StartFrame", "0", NUM32, 0, "", 0, 0x7fffffff},
        {"EndFrame", "400", NUM32, 400, "", 0, 0x7fffffff},
        {"IDRPeriod", "1", NUM32, 1, "", 0, 0x7fffffff},
        {"IPeriod", "0", NUM32, 0, "", 0, 0x7fffffff},
        {"SliceMode", "0", NUM8, 0, "", 0, 4},
        {"NumRefFrames", "1", NUM8, 1, "", 1, 15},
        {"QI", "31", NUM8, 31, "", 0, 51},
        {"QP", "31", NUM8, 31, "", 0, 51},
        {"SliceWidthBits", "400", NUM32, 400, "", 1,
         0x7fffffff},
        {"SliceWidthMB", "12", NUM16, 12, "", 1, 30000},
        {"NumSliceGroups", "0", NUM8, 0, "", 0, 4},
        {"SliceGroupMapType", "0", NUM8, 0, "", 0, 6},
        {"SliceGroupConfigFileName", "sg0conf.cfg", STR, NA,
         "sg0conf.cfg", NA, NA},
        {"SliceGroupSlicingMode", "0", NUM8, 0, "",
         0, 2},
        {"Inter16x16", "ON", NUM8, ON, "", OFF,
         ON},
        {"Inter16x8", "ON", NUM8, ON, "", OFF, ON},
        {"Inter8x16", "ON", NUM8, ON, "", OFF, ON},
        {"Inter8x8", "ON", NUM8, ON, "", OFF, ON},
        {"Inter8x4", "ON", NUM8, ON, "", OFF, ON},
        {"Inter4x8", "ON", NUM8, ON, "", OFF, ON},
        {"Inter4x4", "ON", NUM8, ON, "", OFF, ON},
        {"RDMode", "1", NUM8, 1, "", 0, 2},
        {"OutputMode", "0", NUM8, 0, "", 0, 0},
        {"RCMethod", "2", NUM8, 2, "", 0, 2},
        {NULL, NULL, -1, -1, NULL, 0, 0}
    };
    Map[i++].Location  &o->ctlFile;
    Map[i++].Location  &o->version;
    Map[i++].Location  &o->sourceFilename;
    Map[i++].Location  &o->bitFilename;
    Map[i++].Location  &o->traceFilename;
    Map[i++].Location  &o->reconFilename;
    Map[i++].Location  &o->traceFlag;
    Map[i++].Location  &o->writeReconstructedFlag;
    Map[i++].Location  &e->sourceFrameRate;
    Map[i++].Location  &e->encodingFrameRate;
    Map[i++].Location  &e->targetBitRate;
    Map[i++].Location  &e->qualityTradeoff;
    Map[i++].Location  &e->delayFactor;
    Map[i++].Location  &e->intraRefreshMethod;
    Map[i++].Location  &e->numForcedIntra;
    Map[i++].Location  &e->ME_AlgorithmID;
    Map[i++].Location  &e->mbIntegerSearchDistance;
    Map[i++].Location  &e->level;
    Map[i++].Location  &o->startFrameNum;
    Map[i++].Location  &o->endFrameNum;
    Map[i++].Location  &e->IDRPeriod;
    Map[i++].Location  &e->intraPeriod;
    Map[i++].Location  &e->sliceMode;
    Map[i++].Location  &e->maxRefFrames;
    Map[i++].Location  &e->qi;
    Map[i++].Location  &e->qp;
    Map[i++].Location  &e->resyncMarkSpacing;
    Map[i++].Location  &e->resyncMarkMBSpacing;
    Map[i++].Location  &e->num_slice_groups;
    Map[i++].Location  &e->slice_group_map_type;
    Map[i++].Location  &o->SliceGroupConfigFileName;
    Map[i++].Location  &e->sliceGroupSlicingMode;
    Map[i++].Location  &e->enableBlockSize[0];
    Map[i++].Location  &e->enableBlockSize[1];
    Map[i++].Location  &e->enableBlockSize[2];
    Map[i++].Location  &e->enableBlockSize[3];
    Map[i++].Location  &e->enableBlockSize[4];
    Map[i++].Location  &e->enableBlockSize[5];
    Map[i++].Location  &e->enableBlockSize[6];
    Map[i++].Location  &e->rdMode;
    Map[i++].Location  &o->outputMode;
    Map[i++].Location  &e->rcMethod;
#endif

    //! Set the encoder default values for all parameters above
    i  0;
    while (Map[i].TokenName ! NULL)
    {
        switch (Map[i].Type)
        {
            case NUM32:                        // Numerical
                *(long *) (Map[i].Location)  (long) Map[i].intVal;
                break;
            case NUM16:                        // Numerical
                *(short *) (Map[i].Location)  (short) Map[i].intVal;
                break;
            case NUM8:                         // Numerical
                *(char *) (Map[i].Location)  (char) Map[i].intVal;
                break;
            case STR:
                strcpy((char *) Map[i].Location, Map[i].charVal);
                break;
        }
        i++;
    }

    ReadUserInput(argc, argv, Map);

    // For slice groups
    if ((e->sliceMode  4) && (e->num_slice_groups > 1)
        && ((e->slice_group_map_type  0)
            || (e->slice_group_map_type  2)
            || (e->slice_group_map_type  6)))
    {
        FILE   *sgfile;

        e->sliceGroupRunLength 
            malloc(sizeof (e->sliceGroupRunLength[0]) *
                   e->num_slice_groups);

        if ((sgfile  fopen(o->SliceGroupConfigFileName, "r"))  NULL)
        {
            printf("Unable to open Slice Group Config file %s\n",
                   o->SliceGroupConfigFileName);
            exit(1);
        }
        Print("> Parsing Slice Group Config file %s\n",
              o->SliceGroupConfigFileName);
        if (e->slice_group_map_type  0)
        {
            Print("  > Slice Group Run Lengths :");
            // each line contains one 'run_length' value
            for (i  0; i < e->num_slice_groups; i++)
            {
                fscanf(sgfile, "%hd", (e->sliceGroupRunLength + i));
                fscanf(sgfile, "%*[^\n]");      // Ignore rest of characters in the line
                Print(" %d,", e->sliceGroupRunLength[i]);
            }
            Print("\n\n");
        }
        fclose(sgfile);
    }
    // For only 1st frame being I frame
    if (e->intraPeriod  0)
        e->intraPeriod  0x7fffffff;
}
