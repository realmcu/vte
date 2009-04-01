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
Konstantin L           17/05/2004      ?????????   Initial version 

Irina Inkina           27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#ifndef cursor_h_included
#define cursor_h_included

#include <qwidget.h>

class CursorView : public QWidget
{
Q_OBJECT
 public slots:
   void keyPressEvent(QKeyEvent *e);
    void exitFail();
    void exitPass();
   
 public:
 int VT_rv;
    CursorView(QWidget *parent=NULL,const char *name=NULL);
 private:
   void contextMenuEvent( QContextMenuEvent * );
    
};

#endif
