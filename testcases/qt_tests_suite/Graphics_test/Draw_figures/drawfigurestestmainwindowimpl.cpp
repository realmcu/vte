/*================================================================================================*/
/**
    @file   drawfigurestestmainwindowimpl.cpp

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

#include "drawfigurestestmainwindowimpl.h"
#include <qlayout.h> 
#include <qlabel.h> 
#include "stdlib.h"

DrawFiguresTestMainWindowImpl::DrawFiguresTestMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: DrawFiguresTestMainWindow( parent, name, f ), color("blue")
{
	setCaption("DrawFiguresTest");

	// Add your code

	QAction *fileExitPassAction, *fileExitFailAction;
	QAction *optionsPenColor, *optionsPenWidth, *optionsRandom;
	QAction *drawDRect, *drawDRoundRect, *drawDEllipse, *drawDPie;
	QAction *drawDPoint, *drawDLine, *drawDArc, *drawDChord;
	QAction *drawDLineSegment, *drawDPolyLine, *drawDPolygon;

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

    optionsPenWidth = new QAction("Pen Width", "Pen&Width", CTRL+Key_W, this, "penwidth" );
    connect( optionsPenWidth, SIGNAL(activated()), this, SLOT(setWidth()));

    QPopupMenu * options = new QPopupMenu( this );
	menuBar()->insertSeparator(); 
    menuBar()->insertItem( "&Options", options );
    optionsPenColor->addTo( options );
    optionsPenWidth->addTo( options );

// menu DRAW
    drawDPoint = new QAction("Point", "Point", CTRL+Key_I, this, "drawpoint");
    connect( drawDPoint, SIGNAL(activated()), this, SLOT(drawPoint()));

    drawDLine = new QAction("Line", "Line", CTRL+Key_N, this, "drawline");
    connect( drawDLine, SIGNAL(activated()), this, SLOT(drawLine()));

    drawDRect = new QAction("Rect", "Rect", CTRL+Key_T, this, "drawrect");
    connect( drawDRect, SIGNAL(activated()), this, SLOT(drawRect()));

    drawDRoundRect = new QAction("Round Rect", "RoundRect", CTRL+Key_R, this, "drawroundrect");
    connect( drawDRoundRect, SIGNAL(activated()), this, SLOT(drawRoundRect()));

    drawDEllipse = new QAction("Ellipse", "Ellipse", CTRL+Key_L, this, "drawellipse");
    connect( drawDEllipse, SIGNAL(activated()), this, SLOT(drawEllipse()));

    drawDArc = new QAction("Arc", "Arc", CTRL+Key_A, this, "drawarc");
    connect( drawDArc, SIGNAL(activated()), this, SLOT(drawArc()));
	
    drawDPie = new QAction("Pie", "Pie", CTRL+Key_P, this, "drawpie");
    connect( drawDPie, SIGNAL(activated()), this, SLOT(drawPie()));
	
    drawDChord = new QAction("Chord", "Chord", CTRL+Key_H, this, "drawchord");
    connect( drawDChord, SIGNAL(activated()), this, SLOT(drawChord()));
    
	drawDLineSegment = new QAction("Line Segment", "Line Segment", CTRL+Key_S, this, "drawlinesegment");
    connect( drawDLineSegment, SIGNAL(activated()), this, SLOT(drawLineSegment()));
	
    drawDPolyLine = new QAction("Poly Line", "Poly Line", CTRL+Key_O, this, "drawpolyline");
    connect( drawDPolyLine, SIGNAL(activated()), this, SLOT(drawPolyLine()));
	
    drawDPolygon = new QAction("Polygon", "Polygon", CTRL+Key_G, this, "drawpolygon");
    connect( drawDPolygon, SIGNAL(activated()), this, SLOT(drawPolygon()));


    QPopupMenu * draw = new QPopupMenu( this );
	menuBar()->insertSeparator(); 
    menuBar()->insertItem( "&Draw", draw);
    drawDPoint->addTo( draw );
    drawDLine->addTo( draw );
    drawDRect->addTo( draw );
    drawDRoundRect->addTo( draw );
    drawDEllipse->addTo( draw );
    drawDArc->addTo( draw );
    drawDPie->addTo( draw );
    drawDChord->addTo( draw );
    drawDLineSegment->addTo( draw );
    drawDPolyLine->addTo( draw );
    drawDPolygon->addTo( draw );

	setFixedSize(240,320);
//	setMinimumSize(240,320);
	width = 1;



	bool v = p.begin(this);
	ww=230;//;p.window().width();
	hh=260;//p.window().height();



}

void DrawFiguresTestMainWindowImpl::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void DrawFiguresTestMainWindowImpl::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


void DrawFiguresTestMainWindowImpl::paintEvent( QPaintEvent* e)
{
//	setFixedSize(900,600);
    QWidget::paintEvent( e );

}

void DrawFiguresTestMainWindowImpl::setColor()
{
	//clearScreen();
    QColor oldColor(yellow);
    color = QColorDialog::getColor(color, this);
    if ( !color.isValid() ) color = oldColor;

}

void DrawFiguresTestMainWindowImpl::setWidth()
{
	//clearScreen();
	width = QInputDialog::getInteger ("Pen Width", "width", width, 0, 20);

}

void DrawFiguresTestMainWindowImpl::drawPoint()
{
//	clearScreen();

	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);

	mix();

	p.drawPoint(x1,y1);

	
}

void DrawFiguresTestMainWindowImpl::drawLine()
{
//	clearScreen();

	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);

	mix();

    p.drawLine(x1,y1,x2,y2);	
	
}

void DrawFiguresTestMainWindowImpl::drawRect()
{
//	clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

    p.drawRect(x1,y1,x2,y2);	
	
}

void DrawFiguresTestMainWindowImpl::drawRoundRect()
{
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

    p.drawRoundRect(x1,y1,x2,y2, 25,25);	
	
}

void DrawFiguresTestMainWindowImpl::drawEllipse()
{
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

    p.drawEllipse(x1,y1,x2,y2);	
	
}

void DrawFiguresTestMainWindowImpl::drawArc()
{
	int alen=rand()%5760;
	int angle=rand()%5760;
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

    p.drawArc(x1,y1,x2,y2,angle,alen);	
}

void DrawFiguresTestMainWindowImpl::drawPie()
{
	int alen=rand();
	int angle=rand();
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

    p.drawPie(x1,y1,x2,y2,angle,alen);	


}

void DrawFiguresTestMainWindowImpl::drawChord()
{
	int alen=rand();
	int angle=rand();
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);
	mix();

	p.drawChord(x1,y1,x2,y2,angle,alen);	
	
}

void DrawFiguresTestMainWindowImpl::drawLineSegment()
{
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);

	int size = rand()%32;
//	int size = rand()*0.0002+3;
	int i;
	QPointArray pa(size);
	for(i=0; i<size; i++)
	{
		mix();

		pa.setPoint(i,x1,y1);
		
	}

    p.drawLineSegments(pa);	
	
}

void DrawFiguresTestMainWindowImpl::drawPolyLine()
{
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);

	int size = rand()%43;
	int i;
	QPointArray pa(size);
	for(i=0; i<size; i++)
	{
		mix();
		pa.setPoint(i,x1,y1);
	}
    p.drawPolyline(pa);	


	
}

void DrawFiguresTestMainWindowImpl::drawPolygon()
{
	//clearScreen();
	QPen pen (yellow);
	pen.setColor(color);
	pen.setWidth(width);
	p.setPen(pen);

	int size = rand()%54;
	int i;
	QPointArray pa(size);
	
	for(i=0; i<size; i++)
	{
		mix();
		pa.setPoint(i,x1,y1);
//		pa.point()
	}
    p.drawPolygon(pa);	

	
}


void clearScreen()
{

}

void DrawFiguresTestMainWindowImpl::mix()
{
  int x = 6;
  int y = 32;

  x1=rand()%(ww-40)+x;
  y1=rand()%(hh-40)+y;
  x2=rand()%(ww-x1-5)+5;
  y2=rand()%(hh-y1-5)+5;


}