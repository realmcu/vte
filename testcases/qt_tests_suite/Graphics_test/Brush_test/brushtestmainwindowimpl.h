/*================================================================================================*/
/**
    @file   brushtesmainwindowimpl.h

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

#include "brushtestmainwindow.h"
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qgrid.h>
#include <qpixmap.h>



class BrushTestMainWindowImpl : public BrushTestMainWindow
{
	Q_OBJECT
public:
	BrushTestMainWindowImpl( QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel );


protected:
	void paintEvent( QPaintEvent* e);

public slots:
	void exitPass();
	void exitFail();

private:
	QPixmap stuff;



};

