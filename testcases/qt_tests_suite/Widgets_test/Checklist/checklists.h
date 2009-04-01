/****************************************************************************
** $Id: qt/checklists.h   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   checklist.h

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

#ifndef CHECKLISTS_H
#define CHECKLISTS_H

#include <qwidget.h>
#include <qstatusbar.h> 

class QListView;
class QLabel;

class CheckLists : public QWidget
{
    Q_OBJECT

public slots:
   void keyPressEvent(QKeyEvent *e);
    void exitFail();
    void exitPass();
   
 public:
 int VT_rv;
    CheckLists( QWidget *parent = 0, const char *name = 0 );

protected:
    QListView *lv1, *lv2;
    QLabel *label;
	QStatusBar *statusBar;


protected slots:
    void copy1to2();
    void copy2to3();
private:

   void contextMenuEvent( QContextMenuEvent * );

};

#endif
