#include <qapplication.h>
#include "listbox.h"

extern "C"{
    #include "test.h"
}

int VT_LsitBoxes_cleanup(){
    return TPASS;
};
int VT_LsitBoxes_setup(){
    return TPASS;
};


int VT_LsitBoxes_test(QApplication *app)
{
    ListBoxDemo fm;
    app->setMainWidget(&fm);
    fm.setCaption("LsitBoxes test");// [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
    fm.resize(240,320);
//    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

