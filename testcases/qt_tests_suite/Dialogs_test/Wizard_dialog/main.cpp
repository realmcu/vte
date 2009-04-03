/****************************************************************************
** $Id: main.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "wizard.h"
#include <qapplication.h>

int Wizard_dialog_main(int argc,char **argv)
{
    QApplication a(argc,argv);

    Wizard wizard;
    wizard.setCaption("Qt Example - Wizard");
    wizard.setGeometry(0,0,240,320);
    wizard.show();
    return a.exec();
//11    return wizard.exec();

}
