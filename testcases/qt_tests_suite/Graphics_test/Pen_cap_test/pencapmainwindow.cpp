/****************************************************************************
** Form implementation generated from reading ui file '.\pencapmainwindow.ui'
**
** Created: Wed May 12 14:10:45 2004
**      by: The User Interface Compiler ($Id: pencapmainwindow.cpp,v 1.1.1.1 2008/04/14 09:01:39 b06080 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
/*================================================================================================*/
/**
    @file   pencapmainwindow.cpp

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

#include "pencapmainwindow.h"
#include <qapplication.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qlabel.h>

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

/*
 *  Constructs a PenCapMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
PenCapMainWindow::PenCapMainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
	setFixedSize(240,320);


    (void)statusBar();
    if ( !name )
	setName( "PenCapMainWindow" );

    // toolbars

    languageChange();
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PenCapMainWindow::~PenCapMainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PenCapMainWindow::languageChange()
{
    setCaption( tr( "PenCap" ) );
}

void PenCapMainWindow::paintEvent( QPaintEvent *e )
{
	setFixedSize(240,320);


	QString sn1 = "Cap Style";
	QString sn2 = "Join Style";

    QString s1[3], s2[3];
	s1[0] = "FlatCap";
	s1[1] = "RoundCap";
	s1[2] = "SquareCap";
	s2[0] = "MiterJoin";
	s2[1] = "RoundJoin";
	s2[2] = "BevelJoin";
	
	
	QPainter p( this );
	Qt::PenCapStyle pcs;
	Qt::PenJoinStyle pjs;
    QPen     pen( black, 30 );             // red solid line, 2 pixels wide
	QFont fn ( "Arial", 10, QFont::Bold);
	int i;
	p.setFont(fn);
	p.drawText(10, 40, sn1);
	p.drawText(10, 170, sn2);

	p.setFont(QFont("Arial", 8));


	for(i=0; i<3; i++)
	{
		p.drawText(10, 70+i*30, s1[i]);
		pen.setWidth(11);
		switch(i) {
		case 0:
			pcs = Qt::FlatCap;
			pjs = Qt::MiterJoin;
			break;
		case 1:
			pcs = Qt::RoundCap;
			pjs = Qt::RoundJoin;
			break;
		case 2:
			pcs = Qt::SquareCap;
			pjs = Qt::BevelJoin;
			break;
		default:
			pcs = Qt::FlatCap;
			pjs = Qt::MiterJoin;
			break;
		}

		pen.setCapStyle(pcs);
		p.setPen( pen );
		p.drawLine(120, 65+i*30, 200, 65+i*30);


		p.drawText(10, 210+i*35, s2[i]);
		pen.setWidth(5);
		pen.setJoinStyle(pjs);
		p.setPen( pen );
		p.drawRect(120, 180+i*40, 80, 30);

	}

}


void PenCapMainWindow::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );
//        QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
//	" Menu</b></u></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu; 
}


void PenCapMainWindow::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void PenCapMainWindow::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}

                                                                  
                                                                  

