/*================================================================================================*/
/**
    @file
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
Alexey P           17/05/2004      ?????????   Initial version

Irina Inkina       27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "fonttestmainwindow.h"
#include <qfont.h>
#include <qfontdialog.h>
#include <qaction.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qtextview.h>


class FontTestMainWindowImpl : public FontTestMainWindow
{
	Q_OBJECT

public:
	FontTestMainWindowImpl( QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel );

private:
    QFont curFont;
    QString testString;


protected:
    void paintEvent( QPaintEvent* );

public slots:
	void exitPass();
	void exitFail();
	void chooseFont();



};

