/****************************************************************************
** $Id: main.cpp,v 1.1.1.1 2008/04/14 09:01:41 b06080 Exp $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "scribble.h"
#include "tabletstats.h"
#include <qapplication.h>
#include <qtabwidget.h>


int Tablet_test_main( int argc, char **argv )
{
    QApplication a( argc, argv );
    QTabWidget tab;
    Scribble scribble(&tab, "scribble");
    TabletStats tabStats( &tab, "tablet stats" );
    
    scribble.setMinimumSize( 230,300 );
    tabStats.setMinimumSize( 230,300 );
    tab.addTab(&scribble, "Scribble" );
    tab.addTab(&tabStats, "Tablet Stats" );
    
    a.setMainWidget( &tab );
    
    tab.show();
    return a.exec();
}
