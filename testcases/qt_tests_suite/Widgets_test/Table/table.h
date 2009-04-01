/*================================================================================================*/
/**
    @file   table.h

    @brief  LTP Motorola template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Irina Inkina                27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#ifndef TABLE_H
#define TABLE_H

#include <qwidget.h>
#include <qmenubar.h>
#include <qlabel.h>

#include <qtable.h>

// Qt logo: static const char *qtlogo_xpm[]

//const int numRows = 30;
//const int numCols = 10;

// The program starts here.

class Table : public QWidget
{
  Q_OBJECT

public:
    Table(QWidget* parent=0, const char* name=0);
   // ~Table(){};
public slots:
    void exitFail();
    void exitPass();

private:
   // void contextMenuEvent( QContextMenuEvent * );
};

#endif//