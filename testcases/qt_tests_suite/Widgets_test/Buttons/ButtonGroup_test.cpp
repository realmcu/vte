#include <qapplication.h>
#include "buttongroups.h"

extern "C"{
    #include "test.h"
}

int VT_ButtonGroup_cleanup(){
    return TPASS;
};
int VT_ButtonGroup_setup(){
    return TPASS;
};


int VT_ButtonGroup_test(QApplication *app)
{
    ButtonsGroups fm;
    app->setMainWidget(&fm);
    fm.setCaption("ButtonGroup test");// [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

