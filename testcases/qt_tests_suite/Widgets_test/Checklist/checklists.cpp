/****************************************************************************
** $Id: checklists.cpp,v 1.1.1.1 2008/04/14 09:01:44 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   checklist.cpp

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

#include "checklists.h"
#include <qcursor.h>
#include <qpopupmenu.h>

#include <qlistview.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qvaluelist.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlayout.h>

/*
 * Constructor
 *
 * Create all child widgets of the CheckList Widget
 */

CheckLists::CheckLists( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this, 6, 6);

    QHBoxLayout *lay = new QHBoxLayout( mainLayout );
    lay->setMargin( 5 );

    // create a widget which layouts its childs in a column
    QVBoxLayout *vbox1 = new QVBoxLayout( lay );
    vbox1->setMargin( 5 );

    // First child: a Label
    vbox1->addWidget( new QLabel( "Check some items!", this ) );

    // Second child: the ListView
    lv1 = new QListView( this );
    vbox1->addWidget( lv1 );
    lv1->addColumn( "Items" );
    lv1->setRootIsDecorated( TRUE );

    // create a list with 4 ListViewItems which will be parent items of other ListViewItems
    QValueList<QListViewItem *> parentList;

    parentList.append( new QCheckListItem( lv1, "Parent Item 1", QCheckListItem::CheckBoxController ) );
    parentList.append( new QCheckListItem( lv1, "Parent Item 2", QCheckListItem::CheckBoxController ) );
    parentList.append( new QCheckListItem( lv1, "Parent Item 3", QCheckListItem::CheckBoxController ) );
    parentList.append( new QCheckListItem( lv1, "Parent Item 4", QCheckListItem::CheckBoxController ) );

    QListViewItem *item = 0;
    unsigned int num = 1;
    // go through the list of parent items...
    for ( QValueList<QListViewItem*>::Iterator it = parentList.begin(); it != parentList.end();
	  ( *it )->setOpen( TRUE ), ++it, num++ ) {
	item = *it;
	// ...and create 5 checkable child ListViewItems for each parent item
	for ( unsigned int i = 1; i <= 5; i++ )
	    (void)new QCheckListItem( item, QString( "%1. Child of Parent %2" ).arg( i ).arg( num ), QCheckListItem::CheckBox );
    }


    connect( lv1, SIGNAL( selectionChanged() ),this, SLOT(copy2to3()) );


    statusBar = new QStatusBar(this);
	statusBar->setSizeGripEnabled ( false );

    mainLayout->addWidget(statusBar);



	statusBar->message( "No Item checked  yet...");
	setFixedSize(240,320);
    
}

/*
 * SLOT copy1to2()
 *
 * Copies all checked ListViewItems from the first ListView to
 * the second one, and inserts them as Radio-ListViewItem.
 */

void CheckLists::copy1to2()
{
    // create an iterator which operates on the first ListView
    QListViewItemIterator it( lv1 );

    lv2->clear();

    // Insert first a controller Item into the second ListView. Always if Radio-ListViewItems
    // are inserted into a Listview, the parent item of these MUST be a controller Item!
    QCheckListItem *item = new QCheckListItem( lv2, "Controller", QCheckListItem::Controller );
    item->setOpen( TRUE );

    // iterate through the first ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (QCheckListItem*)it.current() )->isOn() )
		// ...insert a Radio-ListViewItem with the same text into the second ListView
		(void)new QCheckListItem( item, it.current()->text( 0 ), QCheckListItem::RadioButton );

    if ( item->firstChild() )
	( ( QCheckListItem* )item->firstChild() )->setOn( TRUE );
}

/*
 * SLOT copy2to3()
 *
 * Copies the checked item of the second ListView into the
 * Label at the right.
 */

void CheckLists::copy2to3()
{
    // create an iterator which operates on the second ListView
    QListViewItemIterator it( lv1 );


    // iterate through the second ListView...
    for ( ; it.current(); ++it )
	// ...check state of childs, and...
	if ( it.current()->parent() )
	    // ...if the item is checked...
	    if ( ( (QCheckListItem*)it.current() )->isOn() )
		// ...set the text of the item to the label
			statusBar->message(QString("Last checked: ").append( it.current()->text( 0 ) ));
}


extern "C"{
    #include "test.h"
}


extern char *TCID;

void CheckLists::keyPressEvent(QKeyEvent *e){
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

void CheckLists::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
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


void CheckLists::exitPass()
{

	VT_rv=TPASS;
	close();

}

void CheckLists::exitFail()
{
  VT_rv=TFAIL;
	close();

}
