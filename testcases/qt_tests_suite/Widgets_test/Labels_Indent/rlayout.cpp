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
#include "rlayout.h"

RLayout::RLayout(){
	count=0;
	sum=0;
}

void RLayout::add(QWidget *p,int ks){
	w[count]=p;
	k[count]=ks;
	sum+=ks;
	count++;
}

void RLayout::resize(int w,int h){
	int xoff=w/20;
	int ysize=2*h/(2*sum+count+1);
	int yspace=ysize/2;
	int yoff=yspace;
	int xsize=w-2*xoff;

	int y=yoff;
	for(int i=0;i<count;i++){
		this->w[i]->setGeometry(xoff,y,xsize,ysize*k[i]);
		y+=ysize*k[i]+yspace;
	}
}
