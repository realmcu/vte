#include "brushtestmainwindowimpl.h"


BrushTestMainWindowImpl::BrushTestMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: BrushTestMainWindow( parent, name, Qt::WType_Dialog  )
{
	setCaption("BrushTest");

	// Add your code
	QAction * fileExitPassAction, * fileExitFailAction;
//	QAction * fontChooseAction;


    fileExitPassAction = new QAction("ExitPass", "PassE&xit", CTRL+Key_X, this, "exitpass");
    connect( fileExitPassAction, SIGNAL(activated()), this, SLOT(exitPass()));

    fileExitFailAction = new QAction("ExitFail", "Fail&Exit", CTRL+Key_E, this, "exitfail" );
    connect( fileExitFailAction, SIGNAL(activated()), this, SLOT(exitFail()));

    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    fileExitPassAction->addTo( file );
    fileExitFailAction->addTo( file );

	setFixedSize(240,320);



	if (!stuff.load("trolltech2.bmp")) 
	{
	    stuff = QPixmap(20,20);
	    stuff.fill(Qt::green);
	}


}


void BrushTestMainWindowImpl::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void BrushTestMainWindowImpl::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


void BrushTestMainWindowImpl::paintEvent( QPaintEvent* e)
{

    int j=0;
	int i=0;
	QString s[16];
	s[0] = "NoBrush";
	s[1] = "Solid";
	s[2] = "Dense1";
	s[3] = "Dense2";
	s[4] = "Dense3";
	s[5] = "Dense4";
	s[6] = "Dense5";
	s[7] = "Dense6";
	s[8] = "Dense7";
	s[9] = "Hor";
	s[10] = "Ver";
	s[11] = "Cross";
	s[12] = "BDiag";
	s[13] = "FDiag";
	s[14] = "DiagCross";
	s[15] = "Custom";



	QPainter p(this);
	p.setFont(QFont("Arial",8));
	QBrush brush(blue);

    p.setPen( black );            // set black pen, 0 pixel width
	p.setBrush( brush );          // set the yellow brush

	for(j=0;j<4;j++)
		for(i=0; i<4; i++)
		{
			if(j+i!=6)
			{
				brush.setStyle((Qt::BrushStyle) (i*4+j));
				p.setBrush(brush);   
			}
			else
			{
				brush.setPixmap(stuff);
				brush.setStyle(Qt::CustomPattern);
				p.setBrush(brush);   
			}

			p.drawRect( 8+i*56,40+j*68, 48, 48 );
			p.drawText( 8+i*56,36+j*68, s[i*4+j]);

	/*		else
			{
				brush.setPixmap(stuff);
				brush.setStyle(Qt::CustomPattern);
				p.setBrush(brush);   
				p.drawRect( 50+i*100,300, 70,100 );  
				p.drawText( 50+i*100,290, s[i+8]);
			}
	*/
		}


	
}