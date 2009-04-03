/****************************************************************************
** Form implementation generated from reading ui file '.\rectfillingtestmainwindow.ui'
**
** Created: Fri May 14 13:20:18 2004
**      by: The User Interface Compiler ($Id: rectfillingtestmainwindow.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "rectfillingtestmainwindow.h"

#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>

/*
 *  Constructs a RectFillingTestMainWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
RectFillingTestMainWindow::RectFillingTestMainWindow( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "RectFillingTestMainWindow" );

    // toolbars

    languageChange();
    resize( QSize(240,320).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RectFillingTestMainWindow::~RectFillingTestMainWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RectFillingTestMainWindow::languageChange()
{
    setCaption( tr( "RectFillingTest" ) );
}

