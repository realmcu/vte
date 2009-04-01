#include <qwidget.h>
#include <qcolor.h>

class ColorBox:public QWidget{
Q_OBJECT
public:
	QColor color;
	ColorBox(QWidget *parent=NULL,const char *name=NULL);
	void setColor(QColor color);
	virtual void paintEvent(QPaintEvent *pe);
signals:
	void colorChanged();
};

class FunnyBox:public QWidget{
Q_OBJECT
protected:
	
	QColor curColor;
	int curLen;
	void calc(double t);
public:
	QColor beginColor,endColor;
	QCString text;
	
	FunnyBox(QWidget *parent=NULL,const char *name=NULL);
	virtual void paintEvent(QPaintEvent *pe);
	
public slots:
	void setProgress(double t);
};
