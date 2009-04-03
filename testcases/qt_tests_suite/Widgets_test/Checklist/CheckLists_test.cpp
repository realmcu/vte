#include <qapplication.h>
#include "checklists.h"

extern "C"{
    #include "test.h"
}

int VT_CheckLists_cleanup(){
    return TPASS;
};
int VT_CheckLists_setup(){
    return TPASS;
};


int VT_CheckLists_test(QApplication *app)
{
    CheckLists fm;
    app->setMainWidget(&fm);
    fm.setCaption("CheckLists test [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

