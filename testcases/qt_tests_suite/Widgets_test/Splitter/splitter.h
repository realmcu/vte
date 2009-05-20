/****************************************************************************
** $Id: qt/splitter.cpp   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   splitter.h

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


#ifndef SPLITTER_H
#define SPLITTER_H

#include <qwidget.h>
#include <qmenubar.h>
#include <qlabel.h>



////////
class Test : public QWidget {
public:
    Test(QWidget* parent=0, const char* name=0, int f=0);
    void paintEvent(QPaintEvent* e);
private:
};

/////////

class Splitte : public QWidget
{
  Q_OBJECT

public:
    Splitte(QWidget* parent=0, const char* name=0);
    ~Splitte(){};
public slots:
    void exitFail();
    void exitPass();

private:
    void contextMenuEvent( QContextMenuEvent * );
};

#endif//SPLITTER_H
