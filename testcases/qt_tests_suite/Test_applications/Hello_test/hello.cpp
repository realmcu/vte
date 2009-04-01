/****************************************************************************
** $Id: hello.cpp,v 1.1.1.1 2008/04/14 09:01:40 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

/*================================================================================================*/
/**
    @file   hello.cpp

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

#include "hello.h"
#include <qpushbutton.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qapplication.h>

#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

/*
  Constructs a Hello widget. Starts a 40 ms animation timer.
*/

Hello::Hello( const char *text, QWidget *parent, const char *name )
    : QWidget(parent,name), t(text), b(0)
{
    QTimer *timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(animate()) );
    timer->start( 40 );

    resize( 260, 130 );
}


/*
  This private slot is called each time the timer fires.
*/

void Hello::animate()
{
    b = (b + 1) & 15;
    repaint( FALSE );
}


/*
  Handles mouse button release events for the Hello widget.

  We emit the clicked() signal when the mouse is released inside
  the widget.
*/

void Hello::mouseReleaseEvent( QMouseEvent *e )
{
    if ( rect().contains( e->pos() ) )
        emit clicked();
}


/*
  Handles paint events for the Hello widget.

  Flicker-free update. The text is first drawn in the pixmap and the
  pixmap is then blt'ed to the screen.
*/

void Hello::paintEvent( QPaintEvent * )
{
    static int sin_tbl[16] = {
        0, 38, 71, 92, 100, 92, 71, 38,	0, -38, -71, -92, -100, -92, -71, -38};

    if ( t.isEmpty() )
        return;

    // 1: Compute some sizes, positions etc.
    QFontMetrics fm = fontMetrics();
    int w = fm.width(t) + 20;
    int h = fm.height() * 2;
    int pmx = width()/2 - w/2;
    int pmy = height()/2 - h/2;

    // 2: Create the pixmap and fill it with the widget's background
    QPixmap pm( w, h );
    pm.fill( this, pmx, pmy );

    // 3: Paint the pixmap. Cool wave effect
    QPainter p;
    int x = 10;
    int y = h/2 + fm.descent();
    int i = 0;
    p.begin( &pm );
    p.setFont( font() );
    while ( !t[i].isNull() ) {
        int i16 = (b+i) & 15;
        p.setPen( QColor((15-i16)*16,255,255,QColor::Hsv) );
        p.drawText( x, y-sin_tbl[i16]*h/800, t.mid(i,1), 1 );
        x += fm.width( t[i] );
        i++;
    }
    p.end();

    // 4: Copy the pixmap to the Hello widget
    bitBlt( this, pmx, pmy, &pm );
}

void Hello::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_F11);//CTRL+Key_F11
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_F12);//CTRL+Key_F12
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void Hello::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
  qApp->exit(0);
}

void Hello::exitFail()
{
 	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
  qApp->exit(1);
}

