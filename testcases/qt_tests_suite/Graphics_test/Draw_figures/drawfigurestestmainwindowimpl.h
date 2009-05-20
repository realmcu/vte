/*================================================================================================*/
/**
    @file   drawfigurestestmainwindowimpl.h

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

#include "drawfigurestestmainwindow.h"
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qpen.h>
#include <qinputdialog.h>
#include <time.h>



class DrawFiguresTestMainWindowImpl : public DrawFiguresTestMainWindow
{
	Q_OBJECT
public:
	DrawFiguresTestMainWindowImpl( QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel );

protected:
	void paintEvent( QPaintEvent* e);

public slots:
	void exitPass();
	void exitFail();
	void setColor();
	void setWidth();
	void drawPoint();
	void drawLine();
	void drawRect();
	void drawRoundRect();
	void drawEllipse();
	void drawArc();
	void drawPie();
	void drawChord();
	void drawLineSegment();
	void drawPolyLine();
	void drawPolygon();

private:
	QColor color;
	int width;

	QPainter p;

	int ww,hh;
	int x1,y1,x2,y2;
protected:
	QPixmap buffer;
	void mix();

};




