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
#include "rectfillingtestmainwindow.h"
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


class RectFillingTestMainWindowImpl : public RectFillingTestMainWindow
{
	Q_OBJECT
public:
	RectFillingTestMainWindowImpl( QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel );

protected:
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );
	void paintEvent( QPaintEvent* e);
	void drawCoord(int,int,QPoint);

public slots:
	void exitPass();
	void exitFail();
	void setColor();
	void setClipMode();
	void clearScreen();
	void drawRect();

	void testFillRect();
	void testClipRect();
	void testEraseRect();

private:
	QColor color;
	QPainter painter;
	QBrush brush;
    bool mousePressed;
	QPoint startPoint, endPoint, curPoint, prevPoint;
	QRect cliprect, oldcliprect;

	bool isClipMode;
	int  clipmodeID, clipID;


public:
	enum ActionRect  {eFillRect, eClipRect, eEraseRect}; 
	ActionRect actionRect, prevActionRect;
	

};

