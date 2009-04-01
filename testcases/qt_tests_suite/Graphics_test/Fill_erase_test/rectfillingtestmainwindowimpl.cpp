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
#include "rectfillingtestmainwindowimpl.h"


RectFillingTestMainWindowImpl::RectFillingTestMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: RectFillingTestMainWindow( parent, name, f ), brush (blue), painter(this)
{
	setCaption("RectFillingTest");

	// Add your code

	QAction *fileExitPassAction, *fileExitFailAction;
	QAction *optionsPenColor, *optionsClipMode;
	QAction *rectFillRect, *rectClipRect, *rectEraseRect;

// menu FILE
    fileExitPassAction = new QAction("ExitPass", "PassE&xit", CTRL+Key_X, this, "exitpass");
    connect( fileExitPassAction, SIGNAL(activated()), this, SLOT(exitPass()));

    fileExitFailAction = new QAction("ExitFail", "Fail&Exit", CTRL+Key_E, this, "exitfail" );
    connect( fileExitFailAction, SIGNAL(activated()), this, SLOT(exitFail()));

    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    fileExitPassAction->addTo( file );
    fileExitFailAction->addTo( file );

// menu OPTIONS
    optionsPenColor = new QAction("Pen Color", "Pen&Color", CTRL+Key_C, this, "pencolor");
    connect( optionsPenColor, SIGNAL(activated()), this, SLOT(setColor()));

//	optionsClipMode = new QAction("Clip Mode", "Clip &Mode", CTRL+Key_M, this, "clipmode" );
//	connect( optionsClipMode, SIGNAL(activated()), this, SLOT(setClipMode()));

    QPopupMenu * options = new QPopupMenu( this );
//	options->setCheckable( TRUE );

	menuBar()->insertSeparator(); 
    menuBar()->insertItem( "&Options", options );
    optionsPenColor->addTo( options );
//	optionsClipMode->addTo( options );
//	clipmodeID = options->idAt(1);

// menu Rect
    rectFillRect = new QAction("Fill", "FillRect", CTRL+Key_I, this, "fillrect");
    connect( rectFillRect, SIGNAL(activated()), this, SLOT(testFillRect()));

    rectClipRect = new QAction("Clip", "ClipRect", CTRL+Key_R, this, "cliprect");
    connect( rectClipRect, SIGNAL(activated()), this, SLOT(testClipRect()));

    rectEraseRect = new QAction("Erase", "EraseRect", CTRL+Key_L, this, "eraserect");
    connect( rectEraseRect, SIGNAL(activated()), this, SLOT(testEraseRect()));

    QPopupMenu * rect = new QPopupMenu( this );
    rect->insertTearOffHandle();
	rect->setCheckable( TRUE );

	menuBar()->insertSeparator(); 
    menuBar()->insertItem( "&Rect", rect);
    rectFillRect->addTo( rect );
    rectEraseRect->addTo( rect );
    rectClipRect->addTo( rect );

	clipID = rect->idAt(3);
    isClipMode = FALSE;
//	menuBar()->setItemEnabled(clipID, isClipMode);
	QString str;
	str = " Clip Mode = FALSE";
	setCaption("   Current Action: FILLING,  "+str);


	setMaximumSize(240,320);
//	setMinimumSize(850,350);
    mousePressed = FALSE;


	actionRect = eFillRect;
	prevActionRect = eFillRect;

	startPoint.setX(50);
	startPoint.setY(50);
	endPoint.setX(850);
	endPoint.setY(550);
	prevPoint = startPoint;
	color.setNamedColor("magenta");
	drawRect();

}

void RectFillingTestMainWindowImpl::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void RectFillingTestMainWindowImpl::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


void RectFillingTestMainWindowImpl::paintEvent( QPaintEvent* e)
{
//	setFixedSize(900,600);
    QWidget::paintEvent( e );
}

void RectFillingTestMainWindowImpl::setColor()
{
	clearScreen();
    QColor oldColor(yellow);
    color = QColorDialog::getColor(color, this);
    if ( !color.isValid() ) color = oldColor;

}

void RectFillingTestMainWindowImpl::drawRect()
{
//	clearScreen();
//	QPainter p(this);
	QPen pen (yellow);
	QRect rect;
	QColor col(red);
	pen.setColor(color);
	painter.setPen(pen);
	brush.setColor(color);
	painter.setBrush(brush);

	rect.setLeft(startPoint.x());
	rect.setTop(startPoint.y());
	rect.setRight(endPoint.x());
	rect.setBottom(endPoint.y());


	if (actionRect == eClipRect && isClipMode)
	{
		painter.drawWinFocusRect(cliprect);
		painter.setClipRect(rect);
		painter.drawWinFocusRect(rect);
		cliprect = rect;
	}
	else if(actionRect == eEraseRect)
	{
		if (painter.hasClipping()) painter.drawWinFocusRect(cliprect);
		painter.eraseRect(rect);
		if (painter.hasClipping()) painter.drawWinFocusRect(cliprect);
	}
	else  if(actionRect == eFillRect)
	{
		if (painter.hasClipping()) painter.drawWinFocusRect(cliprect);
		painter.fillRect(rect, brush);
		if (painter.hasClipping()) painter.drawWinFocusRect(cliprect);
	}


	
}

void RectFillingTestMainWindowImpl::clearScreen()
{
	repaint( TRUE );
}


void RectFillingTestMainWindowImpl::testFillRect()
{
	actionRect = eFillRect;
	prevActionRect = actionRect;

	QString str;
	if (isClipMode) str = " Clip Mode = TRUE";
	else            str = " Clip Mode = FALSE";
	setCaption("   Current Action: FILLING,  "+str);
}

void RectFillingTestMainWindowImpl::testClipRect()
{
	actionRect = eClipRect;
	isClipMode = !isClipMode;
	menuBar()->setItemChecked(clipID, isClipMode);
	
	if (isClipMode) painter.setClipRect(cliprect);
	else 	actionRect = prevActionRect;

	painter.drawWinFocusRect(cliprect);
	painter.setClipping(isClipMode);
//		painter.drawWinFocusRect(cliprect);

	QString strM, strA;
	strA = "   Current Action: CLIPPING,  ";

	if (isClipMode) strM = " Clip Mode = TRUE";
	else
	{
		if (prevActionRect == eFillRect)       strA = "   Current Action: FILLING,  ";
		else if (prevActionRect == eEraseRect) strA = "   Current Action: ERASING,  ";
		strM = " Clip Mode = FALSE";
	}



	setCaption(strA+strM);

}

void RectFillingTestMainWindowImpl::testEraseRect()
{	
	actionRect = eEraseRect;
	prevActionRect = actionRect;
	QString str;
	if (isClipMode) str = " Clip Mode = TRUE";
	else            str = " Clip Mode = FALSE";
	setCaption("   Current Action: ERASING,  "+str);
}

void RectFillingTestMainWindowImpl::mousePressEvent( QMouseEvent *e )
{
    mousePressed = TRUE;
	startPoint = e->pos();
	prevPoint = startPoint;

}

void RectFillingTestMainWindowImpl::mouseReleaseEvent( QMouseEvent *e )
{
    mousePressed = FALSE;
	endPoint = e->pos();

	QPainter p(this);
	QRect r;

	r.setTopLeft(startPoint);
	r.setBottomRight(prevPoint);
	p.drawWinFocusRect(r);
		

	drawRect();
}

void RectFillingTestMainWindowImpl::mouseMoveEvent( QMouseEvent *e )
{
	QPainter p(this);
	QRect r;


    if ( mousePressed )
	{

		r.setTopLeft(startPoint);
		r.setBottomRight(prevPoint);
		p.drawWinFocusRect(r);
//		p.drawRect(r);
		prevPoint = e->pos();
		r.setBottomRight(prevPoint);
		p.drawWinFocusRect(r);
//		p.drawRect(r);
//		p.fillRect(r);
//		p.drawWinFocusRect(r);
		
    }
}

void RectFillingTestMainWindowImpl::resizeEvent( QResizeEvent *e )
{
    QWidget::resizeEvent( e );
}

void RectFillingTestMainWindowImpl::drawCoord(int x, int y, QPoint point)
{
	QPainter p(this);
	p.eraseRect(0,0,800,50);
	QString s;
	s = "x="+QString::number(point.x())+" y="+QString::number(point.y());
	p.drawText(x,y,s);
}


void RectFillingTestMainWindowImpl::setClipMode()
{
/*
	isClipMode = !isClipMode;
	menuBar()->setItemChecked(clipmodeID, isClipMode);
	menuBar()->setItemEnabled(clipID, isClipMode);
	
	if (!isClipMode)
	{
		painter.drawWinFocusRect(cliprect);
		painter.setClipping(isClipMode);
//		painter.drawWinFocusRect(cliprect);
	}
*/
}
