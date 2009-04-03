/****************************************************************************
** $Id: themes.cpp,v 1.1.1.1 2008/04/14 09:01:41 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   themas.cpp

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

#include "themes.h"
#include "wood.h"
#include "metal.h"

#include "buttons_page_ext.h"

#include <qtabwidget.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qfont.h>
#include <qstylefactory.h>
#include <qaction.h>
#include <qsignalmapper.h>
#include <qdict.h>

Themes::Themes( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f )
{
	QApplication::setFont(QFont("Arial",8),true);
    appFont = QApplication::font();
    tabwidget = new QTabWidget( this );

    tabwidget->addTab( new buttons_page_ext( tabwidget ), "Buttons/Groups" );

    setCentralWidget( tabwidget );

    QPopupMenu *style = new QPopupMenu( this );
    style->setCheckable( TRUE );
    menuBar()->insertItem( "&Style" , style );

    style->setCheckable( TRUE );
    QActionGroup *ag = new QActionGroup( this, 0 );
    ag->setExclusive( TRUE );
    QSignalMapper *styleMapper = new QSignalMapper( this );
    connect( styleMapper, SIGNAL( mapped( const QString& ) ), this, SLOT( makeStyle( const QString& ) ) );
    QStringList list = QStyleFactory::keys();
    list.sort();
#ifndef QT_NO_STYLE_WINDOWS
    list.insert(list.begin(), "Norwegian Wood");
    list.insert(list.begin(), "Metal");
#endif
    QDict<int> stylesDict( 17, FALSE );
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
	QString styleStr = *it;
	QString styleAccel = styleStr;
	if ( stylesDict[styleAccel.left(1)] ) {
	    for ( uint i = 0; i < styleAccel.length(); i++ ) {
		if ( !stylesDict[styleAccel.mid( i, 1 )] ) {
		    stylesDict.insert(styleAccel.mid( i, 1 ), (const int *)1);
		    styleAccel = styleAccel.insert( i, '&' );
		    break;
		}
	    }
	} else {
	    stylesDict.insert(styleAccel.left(1), (const int *)1);
	    styleAccel = "&"+styleAccel;
	}
	QAction *a = new QAction( styleStr, QIconSet(), styleAccel, 0, ag, 0, ag->isExclusive() );
	connect( a, SIGNAL( activated() ), styleMapper, SLOT(map()) );
	styleMapper->setMapping( a, a->text() );
    }
    ag->addTo(style);
    style->insertSeparator();
//    style->insertItem("&Quit", qApp, SLOT( quit() ), CTRL | Key_Q );
    style->insertItem("&Quit - pass", this, SLOT( exitPass() ), CTRL+Key_F11 );
    style->insertItem("&Exit - fail", this, SLOT( exitFail() ), CTRL+Key_F12 );

    QPopupMenu * help = new QPopupMenu( this );
    menuBar()->insertSeparator();
    menuBar()->insertItem( "&Help", help );
    help->insertItem( "&About", this, SLOT(about()), Key_F1);
    help->insertItem( "About &Qt", this, SLOT(aboutQt()));

//1    setFixedSize(240,320);
#ifndef QT_NO_STYLE_WINDOWS
    qApp->setStyle( new NorwegianWoodStyle );
#endif
}

void Themes::makeStyle(const QString &style)
{
    if(style == "Norwegian Wood") {
#ifndef QT_NO_STYLE_WINDOWS
	qApp->setStyle( new NorwegianWoodStyle );
#endif
    } else if( style == "Metal" ) {
#ifndef QT_NO_STYLE_WINDOWS
	qApp->setStyle( new MetalStyle );
#endif
    } else {
	qApp->setStyle(style);
	if(style == "Platinum") {
	    QPalette p( QColor( 239, 239, 239 ) );
	    qApp->setPalette( p, TRUE );
	    qApp->setFont( appFont, TRUE );
	} else if(style == "Windows") {
	    qApp->setFont( appFont, TRUE );
	} else if(style == "CDE") {
	    QPalette p( QColor( 75, 123, 130 ) );
	    p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
	    p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
	    p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
	    p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
	    p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
	    p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
	    p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
	    p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
	    p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
	    p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
	    p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
	    p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
	    p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
	    qApp->setPalette( p, TRUE );
	    qApp->setFont( QFont( "times", appFont.pointSize() ), TRUE );
	} else if(style == "Motif" || style == "MotifPlus") {
	    QPalette p( QColor( 192, 192, 192 ) );
	    qApp->setPalette( p, TRUE );
	    qApp->setFont( appFont, TRUE );
	}
    }
}

void Themes::about()
{
    QMessageBox::about( this, "Qt Controls Example",
			"<p>This example demonstrates the concept of "
			"<b>generalized GUI styles </b> first introduced "
			" with the 2.0 release of Qt.</p>" );
}


void Themes::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Controls Example" );
}



extern "C"{
    #include "test.h"
}


extern char *TCID;

void Themes::keyPressEvent(QKeyEvent *e){
    if((e->key() == Qt::Key_F11)&&(e->state() & ControlButton )){
	VT_rv=TPASS;
	e->accept();
	close();
	return;
    }
    if((e->key() == Qt::Key_F12)&&(e->state() & ControlButton)){
	VT_rv=TFAIL;
	e->accept();
	close();
	return;
    }
    e->ignore();
}

void Themes::exitPass()
{

	VT_rv=TPASS;
//	close();
 qApp->quit();

}

void Themes::exitFail()
{
  VT_rv=TFAIL; qApp->quit();
	///close();
}
