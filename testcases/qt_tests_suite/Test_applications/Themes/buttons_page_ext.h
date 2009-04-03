
#ifndef buttons_page_ext_included
#define buttons_page_ext_included

#include "buttons_page.h"
#include "aux_widgets.h"
#include "qframe.h"
#include "qprogressbar.h"
#include "qlayout.h"
#include "qpushbutton.h"
#include "qbuttongroup.h"

class buttons_page_ext:public buttons_page{
Q_OBJECT
public:

	double t;

	int toggleButtonIndex;

	ColorBox *cbox;
	FunnyBox *fbox;

	QHBoxLayout *toggleLayout;
	QPushButton *toggleButtons[16];
	int toggleButtonCount;

	buttons_page_ext(QWidget *parent=NULL,const char *name=NULL);

	virtual void set_red();
	virtual void set_green();
	virtual void set_blue();

	virtual void auto_clicked();
	virtual void auto_released();
	virtual void auto_pressed();
	virtual void reset_clicked();

	void setToggleButtonIndex(int i);

};

#endif

