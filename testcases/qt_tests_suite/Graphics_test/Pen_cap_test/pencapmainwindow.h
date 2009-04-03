/****************************************************************************
** Form interface generated from reading ui file '.\pencapmainwindow.ui'
**
** Created: Wed May 12 14:10:45 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
/*================================================================================================*/
/**
    @file   pencapmainwindow.h

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

#ifndef PENCAPMAINWINDOW_H
#define PENCAPMAINWINDOW_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;

class PenCapMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    PenCapMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~PenCapMainWindow();

public slots:
    void exitFail();
    void exitPass();

private:
    void contextMenuEvent( QContextMenuEvent * );

protected:
    void paintEvent( QPaintEvent *e );

protected slots:
    virtual void languageChange();

};



#endif // PENCAPMAINWINDOW_H
