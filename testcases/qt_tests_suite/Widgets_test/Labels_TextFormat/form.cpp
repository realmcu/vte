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
#include "form.h"
#include "qlabel.h"
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qapplication.h>

int xsize=500;
int ysize=20;
int yspace=10;
int xoff=10;
int yoff=10;

void Form::resizeEvent(QResizeEvent *e){
	QWidget::resizeEvent(e);
	r.resize(width(),height());
}

Form::Form(QWidget *parent,const char *name):QWidget(parent,name){

	QApplication::setFont(QFont("Arial",8),true);


	QLabel *l;
    
////////////////////////////////////
	l=new QLabel(this);
	r.add(l,3);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|WordBreak);

	l->setTextFormat(PlainText);
	l->setText("Using PlainText format. All formating symbols are visible.<br>\n\
<b>This</b> is a <font color=red> rich </font> <i>text</i> <u>format</u>");

////////////////////////////////////

	l=new QLabel(this);
	r.add(l,3);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|WordBreak);

	l->setTextFormat(RichText);
	l->setText("Using RichText format. All formating symbols must be interprtated in right way.<br>\n\
<b>This</b> is a <font color=red> rich </font> <i>text</i> <u>format</u>");

////////////////////////////////////
	l=new QLabel(this);
	r.add(l,3);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|WordBreak);

	l->setTextFormat(AutoText);
	l->setText("Using AutoText format. Behaviour depends on style stettings.<br>\n\
<b>This</b> is a <font color=red> rich </font> <i>text</i> <u>format</u>");
}



extern "C"{
    #include "test.h"
}


extern char *TCID;

void Form::keyPressEvent(QKeyEvent *e){
    if((e->key() == Qt::Key_F11)&&(e->state() & ControlButton )){
	VT_rv=TPASS;
	e->accept();
	close();
	return;
    }
    if((e->key() == Qt::Key_F12)&&(e->state() & ControlButton)){
	VT_rv=TFAIL;
	e->accept();
	close();
	return;
    }
    e->ignore();
}

void Form::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
   QPalette p2(g2,g2,g2);

//	QApplication::setFont(QFont("Arial",10),true);

    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );

  QFont f("Arial",10);//(contextMenu->font());
//  f.setItalic(true);
  contextMenu->setFont(f);
    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_F11);//CTRL+Key_F11
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_F12);//CTRL+Key_F12
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void Form::exitPass()
{

	VT_rv=TPASS;
	close();

}

void Form::exitFail()
{
  VT_rv=TFAIL;
	close();

}


