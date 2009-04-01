/****************************************************************************
** Form implementation generated from reading ui file '.\penmainwindow.ui'
**
** Created: Wed May 12 11:14:21 2004
**      by: The User Interface Compiler ($Id: penmainwindow.cpp,v 1.1.1.1 2008/04/14 09:01:39 b06080 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
/*================================================================================================*/
/**
    @file   penmainwindow.cpp

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

#include "penmainwindow.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qapplication.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qinputdialog.h> 

/*
 *  Constructs a PenMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
PenMainWindow::PenMainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
	setFixedSize(240,320);
    color = black;
	width = 2;

    (void)statusBar();
    if ( !name )
	setName( "PenMainWindow" );

    // toolbars


	QPopupMenu *fileMenu = new QPopupMenu( this );
    fileMenu->insertItem( "&Exit",  this, SLOT(exitPass()), CTRL+Key_E );
    fileMenu->insertItem( "&Qiut", this, SLOT(exitFail()), CTRL+Key_Q );
 
    QPopupMenu *penMenu = new QPopupMenu( this );
    penMenu->insertItem( "Set pen &Color", this, SLOT(setColor()), CTRL+Key_C );
    penMenu->insertItem( "Set pen &Width", this, SLOT(setWidth()), CTRL+Key_W);


    QPopupMenu *helpMenu = new QPopupMenu( this );
    helpMenu->insertItem( "&About", this, SLOT(About()), Key_F1);




    QMenuBar *menu = new QMenuBar( this );
    menu->insertItem( "&File", fileMenu );
	menu->insertItem( "&Pen", penMenu);
    menu->insertItem( "&Help", helpMenu );

 
  

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PenMainWindow::~PenMainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PenMainWindow::languageChange()
{
    setCaption( tr( "QPen Example" ) );
}


void PenMainWindow::paintEvent( QPaintEvent *e )
{
	QString style_name = "Pen Style";
	QString sw = "Pen Width = ";

    QString s[6];
	s[0] = "No Pen";
	s[1] = "SolidLine";
	s[2] = "DashLine";
	s[3] = "DotLine";
	s[4] = "DashDotLine";
	s[5] = "DashDotDotLine";
	
	
	QPainter p( this );
    QPen     pen( color, width );             // red solid line, 2 pixels wide
	QFont fn ( "Arial", 12, QFont::Bold);
	int i;


	fn.setItalic(true);
	fn.setUnderline(true);
	fn.setFamily("Arial");
    p.setPen( red );             // set blue pen, 0 pixel width
	p.setFont(fn);
	
	

	p.drawText(10, 50, style_name);

	pen.setColor(black);


	fn.setItalic(false);
	fn.setUnderline(false);
	fn.setFamily("Times");
	fn.setPointSize(10);
	p.setFont(fn);

	
	p.drawText(10, 290, sw + QString::number(width) );

	for(i=0; i<6; i++)
	{
		pen.setStyle((Qt::PenStyle)i);
		pen.setColor(color);
		p.setPen(blue);
		p.drawText(10, 100+(i*30), s[i]);

		p.setPen(pen);
		p.drawLine(140, 100+(i*30)-5, 220, 100+(i*30)-5);
		
	}


}

void PenMainWindow::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void PenMainWindow::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}

void PenMainWindow::About()
{
	QMessageBox::information(this, "About", "Exiting with test fail");
	qApp->exit(1);
}

void PenMainWindow::setWidth()
{

	width = QInputDialog::getInteger ("Pen Width", "width", width, 0, 20);
	repaint(true);

}
void PenMainWindow::setColor()
{
	QColor oldColor(color);
    color = QColorDialog::getColor(color, this);
    if ( !color.isValid() ) color = oldColor;
	repaint(true);
	
}
