#include <qapplication.h>
#include "lineedits.h"

extern "C"{
    #include "test.h"
}

int VT_LineEdits_cleanup(){
    return TPASS;
};
int VT_LineEdits_setup(){
    return TPASS;
};


int VT_LineEdits_test(QApplication *app)
{
    LineEdits fm;
    app->setMainWidget(&fm);
    fm.setCaption("LineEdits test");// [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
//    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

