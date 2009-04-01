/****************************************************************************
** $Id: qt/menu.h   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   menu.h

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

#ifndef MENU_H
#define MENU_H

#include <qwidget.h>
#include <qmenubar.h>
#include <qlabel.h>


class MenuExample : public QWidget
{
    Q_OBJECT
public:
    MenuExample( QWidget *parent=0, const char *name=0 );

public slots:
    void open();
    void news();
    void save();
    void closeDoc();
    void undo();
    void redo();
    void normal();
    void bold();
    void underline();
    void about();
    void aboutQt();
    void printer();
    void file();
    void fax();
    void printerSetup();
	void exitPass();
	void exitFail();


protected:
    void    resizeEvent( QResizeEvent * );

signals:
    void    explain( const QString& );

private:
    void contextMenuEvent ( QContextMenuEvent * );


    QMenuBar *menu;
    QLabel   *label;
    bool isBold;
    bool isUnderline;
    int boldID, underlineID;
};


#endif // MENU_H
