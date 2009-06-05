/*================================================================================================*/
/**
    @file   
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
Roman Holodov           17/05/2004      ?????????   Initial version 

Irina Inkina            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#ifndef THEMES_H
#define THEMES_H

#include <qmainwindow.h>
#include <qfont.h>

class QTabWidget;

class Themes: public QMainWindow
{
    Q_OBJECT

public slots:
   void keyPressEvent(QKeyEvent *e);
   void exitPass();   
   void exitFail();
 public:
 int VT_rv;
    Themes( QWidget *parent = 0, const char *name = 0, WFlags f = WType_TopLevel );

protected:
    QTabWidget *tabwidget;

protected slots:
    void makeStyle(const QString &);
    void about();
    void aboutQt();

private:
    QFont appFont;
};


#endif
