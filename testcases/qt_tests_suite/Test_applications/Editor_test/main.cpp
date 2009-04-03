/****************************************************************************
** $Id: main.cpp,v 1.1.1.1 2008/04/14 09:01:40 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/


#include <qapplication.h>
#include "textedit.h"

int Editor_main( int argc, char ** argv ) 
{
    QApplication a( argc, argv );
    TextEdit * mw = new TextEdit();
    mw->setCaption( "Richtext Editor" );
    mw->resize( 240,320 );
    mw->show();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
