/****************************************************************************
** $Id: qt/lineedits.h   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   lineedits.h

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

#ifndef LINEDITS_H
#define LINEDITS_H

#include <qgroupbox.h>

class QLineEdit;
class QComboBox;

class LineEdits : public QGroupBox
{
    Q_OBJECT

public slots:
   void keyPressEvent(QKeyEvent *e);
   void exitFail();
   void exitPass();
   
 public:
 int VT_rv;
    LineEdits( QWidget *parent = 0, const char *name = 0 );

protected:
    QLineEdit *lined1, *lined2, *lined3, *lined4, *lined5;
    QComboBox *combo1, *combo2, *combo3, *combo4, *combo5;

protected slots:
    void slotEchoChanged( int );
    void slotValidatorChanged( int );
    void slotAlignmentChanged( int );
    void slotInputMaskChanged( int );
    void slotReadOnlyChanged( int );
 private:

    void contextMenuEvent( QContextMenuEvent * );
    
};

#endif
