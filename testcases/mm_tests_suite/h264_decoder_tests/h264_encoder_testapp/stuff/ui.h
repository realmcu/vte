/*!
 ***********************************************************************
 *  \file
 *     ui.h
 *
 *  \brief
 *     Prototypes for ui.c and definitions of used structures.
 ***********************************************************************
 */

#ifndef _UI_H_
#define _UI_H_

//! Mapping structure used in the user interface
typedef struct
{
    char   *TokenName;                  //!< Data referred to as
    void   *Location;                   //!< Pointer to the location of data
    int     Type;                       //!< Data type (0=int 1=char)
    long    intVal;                     //!< If int: the default value
    char   *charVal;                    //!< If char: the default string
    long    minVal;                     //!< Minimum value type int param can have
    long    maxVal;                     //!< Maximum value type int param can have
}
ParameterMapping;

void    UI_GetUserInput(IOParams *o, AVC_VideoEncodeParamsStruct * e,
                        int argc, char *argv[]);

#endif
