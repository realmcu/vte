/****************************************************************************
** $Id: qt/wizard.h   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   wizard.cpp

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


#ifndef WIZARD_H
#define WIZARD_H

#include <qwizard.h>

class QWidget;
class QHBox;
class QLineEdit;
class QLabel;

class Wizard : public QWizard
{
    Q_OBJECT

public:
    Wizard( QWidget *parent = 0, const char *name = 0 );

    void showPage(QWidget* page);

public slots:
    void exitFail();
    void exitPass();

protected:
    void setupPage1();
    void setupPage2();
    void setupPage3();

    QHBox *page1, *page2, *page3;
    QLineEdit *key, *firstName, *lastName, *address, *phone, *email;
    QLabel *lKey, *lFirstName, *lLastName, *lAddress, *lPhone, *lEmail;

private:
    void contextMenuEvent( QContextMenuEvent * );


protected slots:
    void keyChanged( const QString & );
    void dataChanged( const QString & );

};

#endif
