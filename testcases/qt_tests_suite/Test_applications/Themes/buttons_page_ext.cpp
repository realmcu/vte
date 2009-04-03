#include "buttons_page_ext.h"

buttons_page_ext::buttons_page_ext(QWidget *parent,const char * name):buttons_page(parent,name){

	toggleButtonIndex=0;

	progressBar1->setProgress(-1);

	t=0;

	cbox=new ColorBox((QWidget *)frame1);
	cbox->setGeometry(frame1->contentsRect());
	cbox->color=eraseColor();
	cbox->show();

	fbox=new FunnyBox((QWidget *)frame2);
	fbox->setGeometry(frame2->contentsRect());
	fbox->endColor=QColor(255,16,16);
	fbox->beginColor=QColor(16,16,255);
	fbox->text=" Yabadabadooo!!! ... keep it pressed, keep it pressed";
	fbox->show();

	QRect r=pushButton1->frameGeometry();
	int w=r.width();
	int h=r.height();
	
	r.setX(0);
	r.setY(0);
	r.setWidth(w);
	r.setHeight(h);

	QVBoxLayout *qb=new QVBoxLayout((QWidget *)toggleGroup,0);
	
	qb->setGeometry(toggleGroup->contentsRect());
	
	toggleLayout=new QHBoxLayout(qb,0);
	toggleLayout->setGeometry(toggleGroup->contentsRect());
	
	qb->addItem(new QSpacerItem(1,2));
	qb->addItem(toggleLayout);
	qb->addItem(new QSpacerItem(1,2));


	toggleButtonCount=toggleGroup->contentsRect().width()/w;

	for(int i=0;i<toggleButtonCount;i++){
		QPushButton *b=new QPushButton(toggleGroup);
		
		b->setToggleButton(true);
		b->resize(w,h);

		toggleButtons[i]=b;
		toggleLayout->addItem(new QSpacerItem(2,1));
		toggleLayout->add(b);
	}
	toggleLayout->addItem(new QSpacerItem(2,1));
}

void buttons_page_ext::set_red(){
	cbox->setColor(QColor(255,16,16));
	buttons_page::set_red();
}

void buttons_page_ext::set_green(){
	cbox->setColor(QColor(16,255,16));
	buttons_page::set_green();
}

void buttons_page_ext::set_blue(){
	cbox->setColor(QColor(16,16,255));
	buttons_page::set_blue();
}

void buttons_page_ext::auto_clicked(){
	if(t>1)return;
	t=t+0.01;
	if(t>1)t=1;
	fbox->setProgress(t);
	progressBar1->setProgress((int)(t*progressBar1->totalSteps()));
	
	setToggleButtonIndex((toggleButtonIndex+1)%toggleButtonCount);
}

void buttons_page_ext::auto_released(){
}

void buttons_page_ext::auto_pressed(){
}

void buttons_page_ext::reset_clicked(){
	progressBar1->setProgress(0);
	t=0;
	fbox->setProgress(0);
	setToggleButtonIndex(0);
}

void buttons_page_ext::setToggleButtonIndex(int i){
	toggleButtonIndex=i;
	toggleButtons[i]->animateClick();
}