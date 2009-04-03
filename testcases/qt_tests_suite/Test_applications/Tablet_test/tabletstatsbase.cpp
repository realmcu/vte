/****************************************************************************
** Form implementation generated from reading ui file 'tabletstatsbase.ui'
**
** Created: Thu May 6 11:01:33 2004
**      by: The User Interface Compiler ($Id: tabletstatsbase.cpp,v 1.1.1.1 2008/04/14 09:01:41 b06080 Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "tabletstatsbase.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "tabletstats.h"

/*
 *  Constructs a TabletStatsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
TabletStatsBase::TabletStatsBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "TabletStatsBase" );
    TabletStatsBaseLayout = new QHBoxLayout( this, 11, 6, "TabletStatsBaseLayout"); 

    Layout7 = new QVBoxLayout( 0, 0, 6, "Layout7"); 

    Layout5 = new QHBoxLayout( 0, 0, 6, "Layout5"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    Layout5->addWidget( TextLabel1 );

    lblXPos = new QLabel( this, "lblXPos" );
    Layout5->addWidget( lblXPos );
    Layout7->addLayout( Layout5 );

    Layout4 = new QHBoxLayout( 0, 0, 6, "Layout4"); 

    TextLabel3 = new QLabel( this, "TextLabel3" );
    Layout4->addWidget( TextLabel3 );

    lblYPos = new QLabel( this, "lblYPos" );
    Layout4->addWidget( lblYPos );
    Layout7->addLayout( Layout4 );

    Layout3 = new QHBoxLayout( 0, 0, 6, "Layout3"); 

    TextLabel5 = new QLabel( this, "TextLabel5" );
    Layout3->addWidget( TextLabel5 );

    lblPressure = new QLabel( this, "lblPressure" );
    Layout3->addWidget( lblPressure );
    Layout7->addLayout( Layout3 );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    TextLabel7 = new QLabel( this, "TextLabel7" );
    Layout2->addWidget( TextLabel7 );

    lblDev = new QLabel( this, "lblDev" );
    Layout2->addWidget( lblDev );
    Layout7->addLayout( Layout2 );

    TextLabel9 = new QLabel( this, "TextLabel9" );
    TextLabel9->setAlignment( int( QLabel::AlignAuto | QLabel::AlignCenter ) );
    Layout7->addWidget( TextLabel9 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    TextLabel10 = new QLabel( this, "TextLabel10" );
    Layout1->addWidget( TextLabel10 );

    lblXTilt = new QLabel( this, "lblXTilt" );
    Layout1->addWidget( lblXTilt );

    TextLabel12 = new QLabel( this, "TextLabel12" );
    Layout1->addWidget( TextLabel12 );

    lblYTilt = new QLabel( this, "lblYTilt" );
    Layout1->addWidget( lblYTilt );
    Layout7->addLayout( Layout1 );

    orient = new MyOrientation( this, "orient" );
    Layout7->addWidget( orient );
    TabletStatsBaseLayout->addLayout( Layout7 );

    statCan = new StatsCanvas( this, "statCan" );
    TabletStatsBaseLayout->addWidget( statCan );
    languageChange();
    resize( QSize(657, 527).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( statCan, SIGNAL( signalNewPressure( int ) ), lblPressure, SLOT( setNum(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
TabletStatsBase::~TabletStatsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void TabletStatsBase::languageChange()
{
    setCaption( tr( "Form1" ) );
    TextLabel1->setText( tr( "X Pos:" ) );
    lblXPos->setText( tr( "0" ) );
    TextLabel3->setText( tr( "Y Pos:" ) );
    lblYPos->setText( tr( "0" ) );
    TextLabel5->setText( tr( "Pressure:" ) );
    lblPressure->setText( tr( "0" ) );
    TextLabel7->setText( tr( "Device:" ) );
    lblDev->setText( tr( "0" ) );
    TextLabel9->setText( tr( "Tilt Information" ) );
    TextLabel10->setText( tr( "X Tilt:" ) );
    lblXTilt->setText( tr( "000" ) );
    TextLabel12->setText( tr( "Y Tilt:" ) );
    lblYTilt->setText( tr( "000" ) );
}

