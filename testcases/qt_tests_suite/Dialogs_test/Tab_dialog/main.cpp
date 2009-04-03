/****************************************************************************
** $Id: main.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"
#include <qapplication.h>
#include <qstring.h>

int Tab_dialog_main( int argc, char **argv )
{

    QApplication a( argc, argv );

    TabDialog tabdialog( 0, "tabdialog", QString( argv[0] ) );

    QFont _font(  tabdialog.font() );
    _font.setPointSize( 12);
    _font.setBold( TRUE );
    tabdialog.setFont(_font);             //
     
    tabdialog.resize(240,320);
//    tabdialog.setFixedSize( 240, 320 );

    tabdialog.setOkButton("Exit - Fail");
    tabdialog.setCaption( "Qt Example - Tabbed Dialog" );
    a.setMainWidget( &tabdialog );
    tabdialog.show();

    return a.exec();
}
