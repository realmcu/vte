/****************************************************************************
** Form interface generated from reading ui file '.\penmainwindow.ui'
**
** Created: Wed May 12 11:14:21 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
/*================================================================================================*/
/**
    @file   penmainwindow.h

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

#ifndef PENMAINWINDOW_H
#define PENMAINWINDOW_H

#include <qvariant.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qmessagebox.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QPainter;

class PenMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    PenMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~PenMainWindow();


public slots:
	void exitPass();
	void exitFail();
	void About();
	void setWidth();
	void setColor();

protected:
    void paintEvent( QPaintEvent *e );

protected slots:
    virtual void languageChange();

private:
	QAction * fileCloseAction1, * fileQuitAction;

	QColor color;
	int width;

};

#endif // PENMAINWINDOW_H
