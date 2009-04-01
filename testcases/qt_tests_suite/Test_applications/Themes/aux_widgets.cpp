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
#include "aux_widgets.h"
#include <qpainter.h>

// ColorBox implementation

ColorBox::ColorBox(QWidget *parent,const char *name):QWidget(parent,name){
}

void ColorBox::setColor(QColor color){
	this->color=color;
	paintEvent(NULL);
	colorChanged();
}

void ColorBox::paintEvent(QPaintEvent *pe){
	QPainter p;
	p.begin(this);
	p.fillRect(rect(),QBrush(color));
	p.end();
}

// FunnyBox implementation

FunnyBox::FunnyBox(QWidget* parent, const char *name):QWidget(parent,name){
}

void FunnyBox::paintEvent(QPaintEvent *pe){
	QRect r(rect());
	QPainter p;
	p.begin(this);
	p.setPen(curColor);
	p.eraseRect(rect());

	r.setWidth(curLen);
	p.drawText(r, AlignRight | AlignVCenter, text);
	p.end();
}

int transf(int a,int b,double t){
	return (int)(((double)(b-a))*t) + a;
};

void FunnyBox::calc(double t){
	QFontMetrics fm(font());
	curLen=(int) (((double)fm.width(text)) * t);
	int r=transf(beginColor.red(),endColor.red(),t);
	int g=transf(beginColor.green(),endColor.green(),t);
	int b=transf(beginColor.blue(),endColor.blue(),t);
	curColor.setRgb(r,g,b);
}

void FunnyBox::setProgress(double t){
	calc(t);
	paintEvent(NULL);
}
