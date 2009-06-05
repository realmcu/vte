/****************************************************************************
** $Id: qt/lifedlg.h   3.3.2   edited May 27 2003 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   lifedlg.h

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

#ifndef LIFEDLG_H
#define LIFEDLG_H

#include <qtimer.h>
#include <qwidget.h>

class QSlider;
class QPushButton;
class QLabel;
class QComboBox;

#include "life.h"


class LifeTimer : public QTimer
{
    Q_OBJECT
public:
    LifeTimer( QWidget *parent );
    enum { MAXSPEED = 1000 };

public slots:
    void	setSpeed( int speed );
    void	pause( bool );

private:
    int		interval;
};


class LifeDialog : public QWidget
{
    Q_OBJECT
public:
    LifeDialog( int scale = 10, QWidget *parent = 0, const char *name = 0 );
public slots:
    void	getPattern( int );
    void exitPass();
    void exitFail();
protected:
    virtual void resizeEvent( QResizeEvent * e );

private:
    enum { TOPBORDER = 70, SIDEBORDER = 10 };

    LifeWidget	*life;
    QPushButton *qb;
    QPushButton *qb_1;
    LifeTimer	*timer;
    QPushButton *pb;
    QComboBox	*cb;
    QLabel	*sp;
    QSlider	*scroll;
   void contextMenuEvent( QContextMenuEvent * );    
};


#endif // LIFEDLG_H
