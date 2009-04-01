#include <qapplication.h>
#include "cursor.h"

extern "C"{
    #include "test.h"
}

int VT_CursorView_cleanup(){
    return TPASS;
};
int VT_CursorView_setup(){
    return TPASS;
};


int VT_CursorView_test(QApplication *app)
{
    CursorView fm;
    app->setMainWidget(&fm);
    fm.setCaption("CursorView test");// [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

