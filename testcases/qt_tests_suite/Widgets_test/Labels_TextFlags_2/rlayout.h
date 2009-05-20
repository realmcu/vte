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

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#ifndef rlayout_h_included
#define rlayout_h_included

#include <qwidget.h>

#define MaxW 32

class RLayout{
public:
    QWidget *w[MaxW];
    int k[MaxW];
	int sum;
    int count;

    void add(QWidget *p,int ks=1);
    void resize(int w,int h);

	RLayout();
};

#endif
