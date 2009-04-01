/****************************************************************************
** Form implementation generated from reading ui file '.\fontmetricstestmainwindow.ui'
**
** Created: Thu May 13 11:40:18 2004
**      by: The User Interface Compiler ($Id: fontmetricstestmainwindow.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "fontmetricstestmainwindow.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

/*
 *  Constructs a FontMetricsTestMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
FontMetricsTestMainWindow::FontMetricsTestMainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "FontMetricsTestMainWindow" );

    // toolbars

    languageChange();
    resize( QSize(600, 480).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
FontMetricsTestMainWindow::~FontMetricsTestMainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void FontMetricsTestMainWindow::languageChange()
{
    setCaption( tr( "FontMetricsTest" ) );
}

