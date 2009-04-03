/****************************************************************************
** $Id: tetrix.cpp,v 1.1.1.1 2008/04/14 09:01:44 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "qtetrix.h"
#include "qfont.h"

int Tetrix_main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );

	QApplication a(argc,argv);
    QTetrix *tetrix = new QTetrix;
    tetrix->setCaption("Tetrix");
    a.setMainWidget(tetrix);
    tetrix->setCaption("Qt Example - Tetrix");
    tetrix->show();
    return a.exec();
}
