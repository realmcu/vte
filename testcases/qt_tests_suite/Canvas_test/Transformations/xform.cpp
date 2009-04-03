/*================================================================================================*/
/**
    @file   xform.cpp

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


#include <xform.h> 
#include <qapplication.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>    
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qlcdnumber.h>
#include <qslider.h>
#include <qmenubar.h>
#include <qfontdialog.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qwidgetstack.h> 
#include <qpainter.h>
#include <qpixmap.h>
#include <qpicture.h>     
#include <stdlib.h>



XFormControl::XFormControl( const QFont &initialFont,
			    QWidget *parent, const char *name )
	: QVBox( parent, name )
{

    setSpacing(4);
    setMargin(4);
    currentFont = initialFont;
    mode = Image;

    rotLCD	= new QLCDNumber( 4, this, "rotateLCD" );
    rotLCD->setSmallDecimalPoint ( true );
//    rotLCD->setSegmentStyle(QLCDNumber::Flat);
    rotS	= new QSlider( QSlider::Horizontal, this,
				  "rotateSlider" );
    shearLCD	= new QLCDNumber( 5,this, "shearLCD" );

    shearLCD->setGeometry( 0, 0,20,2);
    shearLCD->setSmallDecimalPoint ( true );
    shearS	= new QSlider( QSlider::Horizontal, this,
				  "shearSlider" );
    mirror	= new QCheckBox( this, "mirrorCheckBox" );
    rb_txt = new QRadioButton( this, "text" );
    rb_img = new QRadioButton( this, "image" );
    rb_pic = new QRadioButton( this, "picture" );
    optionals = new QWidgetStack(this);
    optionals->setGeometry( 4, 2,20,10 );

    QVBox* optionals_text = new QVBox(optionals);
    optionals_text->setSpacing(4);
    QVBox* optionals_other = new QVBox(optionals);
    optionals_other->setSpacing(4);
    optionals->addWidget(optionals_text,0);
    optionals->addWidget(optionals_other,1);
    fpb		= new QPushButton( optionals_text, "text" );
    textEd	= new QLineEdit( optionals_text, "text" );
    textEd->setFocus(); 
    rotLCD->display( "  0'" );

    rotS->setRange( -180, 180 );
    rotS->setValue( 0 );
    connect( rotS, SIGNAL(valueChanged(int)), SLOT(newMtx()) );

    shearLCD->display( "0.00" );

    shearS->setRange( -25, 25 );
    shearS->setValue( 0 );
    connect( shearS, SIGNAL(valueChanged(int)), SLOT(newMtx()) );

    mirror->setText( tr("Mirror") );
    connect( mirror, SIGNAL(clicked()), SLOT(newMtx()) );
 
    QButtonGroup *bg = new QButtonGroup(this);
    bg->hide();
    bg->insert(rb_txt,0);
    bg->insert(rb_img,1);
    bg->insert(rb_pic,2);
    rb_txt->setText( tr("Text") );
    rb_img->setText( tr("Image") );
    rb_img->setChecked(TRUE);
    rb_pic->setText( tr("Picture") );
    connect( bg, SIGNAL(clicked(int)), SLOT(changeMode(int)) );

    fpb->setText( tr("Select font...") );
    connect( fpb, SIGNAL(clicked()), SLOT(selectFont()) );

    textEd->setText( "Troll" );
    connect( textEd, SIGNAL(textChanged(const QString&)),
		     SLOT(newTxt(const QString&)) );

    magLCD = new QLCDNumber( 4,optionals_other, "magLCD" );
    magLCD->display( "100" );
    magLCD->setSmallDecimalPoint ( true );
    magS = new QSlider( QSlider::Horizontal, optionals_other,
			   "magnifySlider" );
    magS->setRange( 0, 800 );
    connect( magS, SIGNAL(valueChanged(int)), SLOT(newMtx()) );
    magS->setValue( 0 );
    connect( magS, SIGNAL(valueChanged(int)), magLCD, SLOT(display(int)));

    optionals_text->adjustSize();
    optionals_other->adjustSize(); 
    changeMode(Image); 
    startTimer(20); // start an initial animation
       
}


void XFormControl::timerEvent(QTimerEvent*)
{
    int v = magS->value();
    v = (v+2)+v/10;
/*111
    if ( v >= 200 ) {
	v = 200;
	killTimers();
    }

*/    
    if ( v >= 50 ) {
	v = 50;
	killTimers();
    }
    magS->setValue(v);
}



/*
    Called whenever the user has changed one of the matrix parameters
    (i.e. rotate, shear or magnification)
*/

void XFormControl::newMtx()
{
    emit newMatrix( matrix() );
}

void XFormControl::newTxt(const QString& s)
{
    emit newText(s);
    changeMode(Text);
}

/*
    Calculates the matrix appropriate for the current controls,
    and updates the displays.
*/

QWMatrix XFormControl::matrix()
{
    QWMatrix m;
    if (mode != Text) {
	double magVal = 1.0*magS->value()/100;
	m.scale( magVal, magVal );
    }
    double shearVal = 1.0*shearS->value()/25;
    m.shear( shearVal, shearVal );
    m.rotate( rotS->value() );
    if ( mirror->isChecked() ) {
	m.scale( 1, -1 );
	m.rotate( 180 );
    }

    QString tmp;
    tmp.sprintf( "%1.2f", shearVal  );
    if ( shearVal >= 0 )
	tmp.insert( 0, " " );
    shearLCD->display( tmp );

    int rot = rotS->value();
    if ( rot < 0 )
	rot = rot + 360;
    tmp.sprintf( "%3i'", rot );
    rotLCD->display( tmp );
    return m;
}


void XFormControl::selectFont()
{
    bool ok;
    QFont f = QFontDialog::getFont( &ok, currentFont );
    if ( ok ) {
	currentFont = f;
	fontSelected( f );
    }
}

void XFormControl::fontSelected( const QFont &font )
{
    emit newFont( font );
    changeMode(Text);
}


/*
    Sets the mode - Text, Image, or Picture.
*/

void XFormControl::changeMode(int m)
{
    mode = (Mode)m;

    emit newMode( m );
    newMtx();
    if ( mode == Text ) {
	optionals->raiseWidget(0);
	rb_txt->setChecked(TRUE);
    } else {
	optionals->raiseWidget(1);
	if ( mode == Image )
	    rb_img->setChecked(TRUE);
	else
	    rb_pic->setChecked(TRUE);
    }
    qApp->flushX();
}    

ShowXForm::ShowXForm( const QFont &initialFont,
		      QWidget *parent, const char *name )
	: QWidget( parent, name, WResizeNoErase )
{
   setFont( initialFont );
    setBackgroundColor( white );
    m = Text;
    eraseRect = QRect( 0, 0, 0, 0 ); 
}

QSizePolicy ShowXForm::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

}

QSize ShowXForm::sizeHint() const
{
//11    return QSize(200/2,300/2);
   return QSize(200,300);

}

void ShowXForm::paintEvent( QPaintEvent * )
{
    showIt();
}

void ShowXForm::resizeEvent( QResizeEvent * )
{
    eraseRect = QRect( width()/2, height()/2, 0, 0 );
    repaint(rect());
}

void ShowXForm::setText( const QString& s )
{
    text = s;
    showIt();
}

void ShowXForm::setMatrix( QWMatrix w )
{
    mtx = w;
    showIt();
}

void ShowXForm::setFont( const QFont &f )
{
    m = Text;
    QWidget::setFont( f );
}

void ShowXForm::setPixmap( QPixmap pm )
{
    pix	 = pm;
    m    = Image;
    showIt();
}

void ShowXForm::setPicture( const QPicture& p )
{
    picture = p;
    m = Picture;
    showIt();
}

void ShowXForm::setMode( int mode )
{
    m = (Mode)mode;
}

void ShowXForm::showIt()
{
    QPainter p;
    QRect r;	  // rectangle covering new text/pixmap in virtual coordinates
    QWMatrix um;  // copy user specified transform
    int textYPos = 0; // distance from boundingRect y pos to baseline
    int textXPos = 0; // distance from boundingRect x pos to text start
    QRect br;
    QFontMetrics fm( fontMetrics() );	// get widget font metrics
    switch ( mode() ) {
      case Text:
	br = fm.boundingRect( text );	// rectangle covering text
	r  = br;
	textYPos = -r.y();
	textXPos = -r.x();
	br.moveTopLeft( QPoint( -br.width()/2, -br.height()/2 ) );
        break;
      case Image:
	r = pix.rect();
        break;
      case Picture:
	// ### need QPicture::boundingRect()
	r = QRect(0,0,100,100);
  
        break;
    }
    r.moveTopLeft( QPoint(-r.width()/2, -r.height()/2) );
	  // compute union of new and old rect
	  // the resulting rectangle will cover what is already displayed
	  // and have room for the new text/pixmap
    eraseRect = eraseRect.unite( mtx.map(r) );
    eraseRect.moveBy( -1, -1 ); // add border for matrix round off
    eraseRect.setSize( QSize( eraseRect.width() + 2,eraseRect.height() + 2 ) );
    int pw = QMIN(eraseRect.width(),width());
    int ph = QMIN(eraseRect.height(),height());
    QPixmap pm( pw, ph );		// off-screen drawing pixmap
    pm.fill( backgroundColor() );

    p.begin( &pm );
    um.translate( pw/2, ph/2 );	// 0,0 is center
    um = mtx * um;
    p.setWorldMatrix( um );
    switch ( mode() ) {
      case Text:
	p.setFont( font() );		// use widget font
	p.drawText( r.left() + textXPos, r.top() + textYPos, text );
#if 0
	p.setPen( red );
	p.drawRect( br );
#endif
	break;
      case Image:
	p.drawPixmap( -pix.width()/2, -pix.height()/2, pix );
	//QPixmap rotated = pix.xForm(mtx);
	//bitBlt( &pm, pm.width()/2 - rotated.width()/2,
		//pm.height()/2 - rotated.height()/2, &rotated );
	break;
      case Picture:
	// ### need QPicture::boundingRect()
	p.scale(0.25,0.25);
	p.translate(-230,-180);
	p.drawPicture( picture );
    }
    p.end();

    int xpos = width()/2  - pw/2;
    int ypos = height()/2 - ph/2;
    bitBlt( this, xpos, ypos,			// copy pixmap to widget
	    &pm, 0, 0, -1, -1 );
    eraseRect =	 mtx.map( r );
}
  

/*
    Grand unifying widget, putting ShowXForm and XFormControl
    together.
*/

void XFormCenter::newMode( int m )
{
    static bool first_i = TRUE;
    static bool first_p = TRUE;

    if ( sx->mode() == m )
	return;
    if ( m == Image && first_i ) {
	first_i = FALSE;
	QPixmap pm;
	if ( pm.load( "image.any" ) )
	    sx->setPixmap( pm );
	return;
    }
    if ( m == Picture && first_p ) {
	first_p = FALSE;
	QPicture p;
	if (p.load( "picture.any" ))
	    sx->setPicture( p );
	return;
    }
    sx->setMode(m);
}


XFormCenter::XFormCenter( QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    resize(240,320);

    QFont f( "Charter", 8, QFont::Bold );
    xc = new XFormControl( f, this );
    sx = new ShowXForm( f, this );
    sx->setText( "Troll" );
    setStretchFactor(sx,1);
    xc->setFrameStyle (QLabel::Panel|QLabel::Raised);
    xc->setLineWidth( 2 );
   connect( xc, SIGNAL(newText(const QString&)), sx,
		 SLOT(setText(const QString&)) );
    connect( xc, SIGNAL(newMatrix(QWMatrix)),
	     sx, SLOT(setMatrix(QWMatrix)) );
    connect( xc, SIGNAL(newFont(const QFont&)), sx,
		 SLOT(setFont(const QFont&)) );
    connect( xc, SIGNAL(newMode(int)), SLOT(newMode(int)) );
    sx->setText( "Troll" );
    newMode( Image );
    sx->setMatrix(xc->matrix()); 
}





void XFormCenter::contextMenuEvent( QContextMenuEvent * )
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
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu; 
}


void XFormCenter::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void XFormCenter::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}



void XFormCenter::setFont( const QFont &f ) 
{ 
  	sx->setFont( f ); 
}
 

int Transformations_main( int argc, char **argv )
{
    QApplication a( argc, argv );

    XFormCenter *xfc = new XFormCenter;
    a.setMainWidget( xfc );
    xfc->setCaption("Qt Example - XForm");
    xfc->show();
    return a.exec();
}




//#include "xform.i"		      // include metadata generated by the moc
