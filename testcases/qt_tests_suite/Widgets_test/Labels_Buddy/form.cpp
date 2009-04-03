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
#include "qlineedit.h"
#include <qapplication.h>
#include <qcursor.h>
#include <qpopupmenu.h>
 
#include "rlayout.h"

#define BuddyP 3
#define BuddyQ 1

class Buddy:public QWidget{
public:
	QLabel *l;
	QLineEdit *e;

	Buddy(QWidget *parent=NULL,const char *name=NULL):QWidget(parent,name){
		l=new QLabel(this);
		e=new QLineEdit(this);
	}
	virtual void resizeEvent(QResizeEvent *e){
		QWidget::resizeEvent(e);
		int xspace=width()/20;
		int lx=((width()-xspace)*BuddyP)/(BuddyP+BuddyQ);
		l->setGeometry(0,0,lx,height());
		this->e->setGeometry(lx+xspace,0,((width()-xspace)*BuddyQ)/(BuddyP+BuddyQ),height());
	}
};


Form::Form(QWidget *parent,const char *name):QWidget(parent,name){
	QApplication::setFont(QFont("Arial",12),true);

  setFixedSize(240,320/2);

	Buddy *b;

////////////////////////////////////
	b=new Buddy(this);
	r.add(b,1);

	b->l->setFrameShape(QFrame::Box);
	b->l->setTextFormat(PlainText);
	b->l->setText("Press Alt-&A");
	b->l->setBuddy(b->e);
	b->e->setText("StringA");


////////////////////////////////////
	b=new Buddy(this);
	r.add(b,1);

	b->l->setFrameShape(QFrame::Box);
	b->l->setTextFormat(PlainText);
	b->l->setText("Press Alt-&B");
	b->l->setBuddy(b->e);
	b->e->setText("StringB");

////////////////////////////////////
	b=new Buddy(this);
	r.add(b,1);

	b->l->setFrameShape(QFrame::Box);
	b->l->setTextFormat(PlainText);
	b->l->setText("Press Alt-&C");
	b->l->setBuddy(b->e);
	b->e->setText("StringC");

////////////////////////////////////
}


void Form::resizeEvent(QResizeEvent *e){
	QWidget::resizeEvent(e);
	int h=QApplication::fontMetrics().height()*6;
	r.resize(width(),h);
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


