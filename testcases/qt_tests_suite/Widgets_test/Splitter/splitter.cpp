/****************************************************************************
** $Id: splitter.cpp,v 1.1.1.1 2008/04/14 09:01:46 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   splitter.cpp

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

#include <qapplication.h>
#include <qlabel.h>
#include <qsplitter.h>
#include <qmultilineedit.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

#include <qcursor.h>
#include <qmessagebox.h>

#include "splitter.h"


Test::Test(QWidget* parent, const char* name, int f) :
    QWidget(parent, name, f)
{

}                          

void Test::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.setClipRect(e->rect());
    const int d = 1000; //large number
    int x1 = 0;
    int x2 = width()-1;
    int y1 = 0;
    int y2 = height()-1;

    int x = (x1+x2)/2;
    p.drawLine( x, y1, x+d, y1+d   );
    p.drawLine( x, y1, x-d, y1+d   );
    p.drawLine( x, y2, x+d, y2-d   );
    p.drawLine( x, y2, x-d, y2-d   );

    int y = (y1+y2)/2;
    p.drawLine( x1, y, x1+d, y+d   );
    p.drawLine( x1, y, x1+d, y-d   );
    p.drawLine( x2, y, x2-d, y+d   );
    p.drawLine( x2, y, x2-d, y-d   );
}

  

//////////

Splitte::Splitte(QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    
    QMenuBar *menu = new QMenuBar( this );
    Q_CHECK_PTR( menu );
    QPopupMenu *file = new QPopupMenu;

    Q_CHECK_PTR( file );
    file->insertSeparator();
    file->insertItem( "&Quit - pass", this, SLOT(exitPass()), CTRL+Key_Q );
    file->insertItem( "E&xit - fail", this, SLOT(exitFail()), CTRL+Key_X );

    menu->insertItem( "&File", file );
                                          
    QSplitter *s1 = new QSplitter( QSplitter::Vertical, this , "main" );
    s1->setMinimumSize( 240, 320 );
    QSplitter *s2 = new QSplitter( QSplitter::Horizontal, s1, "top" );

    Test *t1 = new Test( s2, "topLeft" );
    t1->setBackgroundColor( Qt::blue.light( 180 ) );
    t1->setMinimumSize( 50, 0 );

                     
    Test *t2 = new Test( s2, "topRight" );
    t2->setBackgroundColor( Qt::green.light( 180 ) );
    s2->setResizeMode( t2, QSplitter::KeepSize );
    s2->moveToFirst( t2 );

    QSplitter *s3 = new QSplitter( QSplitter::Horizontal,  s1, "bottom" );

    Test *t3 = new Test( s3, "bottomLeft" );
    t3->setBackgroundColor( Qt::red );
    Test *t4 = new Test( s3, "bottomMiddle" );
    t4->setBackgroundColor( Qt::white );

    Test *t5 = new Test( s3, "bottomRight" );
    t5->setMaximumHeight( 250 );
    t5->setMinimumSize( 80, 50 );
    t5->setBackgroundColor( Qt::yellow );
#ifdef Q_WS_QWS
    // Qt/Embedded XOR drawing not yet implemented.
    s1->setOpaqueResize( TRUE );
#endif
    s2->setOpaqueResize( TRUE );
    s3->setOpaqueResize( TRUE );
      

}

//////////////
void Splitte::contextMenuEvent( QContextMenuEvent * )
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
    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void Splitte::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
  qApp->exit(0);
}

void Splitte::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}

/////////// 
int Splitter_main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    Splitte sd;
    a.setMainWidget(&sd);
    sd.resize(240,320);
    sd.show();
    return a.exec();

}


/*
int Splitter_main( int argc, char ** argv )
{
    QApplication a( argc, argv );

    QSplitter *s1 = new QSplitter( QSplitter::Vertical, 0 , "main" );

    QSplitter *s2 = new QSplitter( QSplitter::Horizontal, s1, "top" );
  
    Test *t1 = new Test( s2, "topLeft" );
    t1->setBackgroundColor( Qt::blue.light( 180 ) );
    t1->setMinimumSize( 50, 0 );


    Test *t2 = new Test( s2, "topRight" );
    t2->setBackgroundColor( Qt::green.light( 180 ) );
    s2->setResizeMode( t2, QSplitter::KeepSize );
    s2->moveToFirst( t2 );

    QSplitter *s3 = new QSplitter( QSplitter::Horizontal,  s1, "bottom" );

    Test *t3 = new Test( s3, "bottomLeft" );
    t3->setBackgroundColor( Qt::red );
    Test *t4 = new Test( s3, "bottomMiddle" );
    t4->setBackgroundColor( Qt::white );

    Test *t5 = new Test( s3, "bottomRight" );
    t5->setMaximumHeight( 250 );
    t5->setMinimumSize( 80, 50 );
    t5->setBackgroundColor( Qt::yellow );
 
#ifdef Q_WS_QWS
    // Qt/Embedded XOR drawing not yet implemented.
    s1->setOpaqueResize( TRUE );
#endif
    s2->setOpaqueResize( TRUE );
    s3->setOpaqueResize( TRUE );

    a.setMainWidget( s1 );
    s1->setCaption("Qt Example - Splitters");
    s1->resize(240,320);
    s1->show();
    int result = a.exec();
    delete s1;
    return result;
    
}                 */
