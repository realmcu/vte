#include <qapplication.h>
#include "themes.h"

extern "C"{
    #include "test.h"
}

int VT_Themes_cleanup(){
    return TPASS;
};
int VT_Themes_setup(){
    return TPASS;
};


int VT_Themes_test(QApplication *app)
{
    Themes fm;
    app->setMainWidget(&fm);
    fm.setCaption("Themes test" );//CTRL+F12 - Fail]");
    fm.resize(240,320);
    fm.show();
//    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

