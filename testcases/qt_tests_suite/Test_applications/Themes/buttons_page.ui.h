/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void buttons_page::bg_clicked( int nid )
{
	QButton *b=buttonGroup1->find(nid);
	if(b==radioButton1){set_red();return;}
	if(b==radioButton2){set_green();return;}
	if(b==radioButton3){set_blue();return;}
}


void buttons_page::ebg_clicked( int nid )
{
	QButton *b=buttonGroup3->find(nid);
	if(b==checkBox1){set_red();return;}
	if(b==checkBox2){set_green();return;}
	if(b==checkBox3){set_blue();return;}
}


void buttons_page::auto_clicked()
{

}


void buttons_page::red_clicked()
{
	set_red();
}


void buttons_page::green_clicked()
{
	set_green();
}


void buttons_page::blue_clicked()
{
	set_blue();
}


void buttons_page::auto_pressed()
{

}


void buttons_page::auto_released()
{

}


void buttons_page::reset_clicked()
{

}


void buttons_page::set_red()
{
	checkBox1->setChecked(true);
	radioButton1->setChecked(true);
}


void buttons_page::set_green()
{
	checkBox2->setChecked(true);
	radioButton2->setChecked(true);
}


void buttons_page::set_blue()
{
	checkBox3->setChecked(true);
	radioButton3->setChecked(true);
}
