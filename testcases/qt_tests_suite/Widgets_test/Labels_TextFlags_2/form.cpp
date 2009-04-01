/*================================================================================================*/
/**
    @file  form.cpp 
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

Irina Inkin            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "form.h"
#include "qlabel.h"
#include "qlabel.h"
#include <qcursor.h>
#include <qpopupmenu.h>



#include <qapplication.h>


void Form::resizeEvent(QResizeEvent *e){
	QWidget::resizeEvent(e);
	r.resize(width(),height());
}

Form::Form(QWidget *parent,const char *name):QWidget(parent,name){

	QApplication::setFont(QFont("Arial",10),true);
setFixedSize(240,320);

    QLabel *l;
    
////////////////////////////////////
	l=new QLabel(this);
	r.add(l,2);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|ExpandTabs);

	l->setText("\tOne TAB step.\nUsing ExpandTabs flag.");

////////////////////////////////////
	l=new QLabel(this);
	r.add(l,2);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop);

	l->setText("\tOne TAB step.\nNo ExpandTab flag.");

////////////////////////////////////
	l=new QLabel(this);
	r.add(l,2);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|ShowPrefix);

	l->setText("Using ShowPrefix flag.\n&Prefix.  Ampersand &&");

////////////////////////////////////
	l=new QLabel(this);
	r.add(l,2);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop|NoAccel);

	l->setText("Using NoAccel flag.\n&Prefix. Ampersand &&");

////////////////////////////////////
	l=new QLabel(this);
	r.add(l,2);
	l->setFrameShape(QFrame::Box);
	l->setAlignment(AlignLeft|AlignTop);

	l->setText("No ShowPrefix flag.\n&Prefix. Ampersand &&");
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


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

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
