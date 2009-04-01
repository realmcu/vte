#include <qapplication.h>
#include "PDTestView.h"

int Stress_test_main(int argc, char* argv[])
{
    QApplication app( argc, argv );

    ProgressDialogTestView fm;
    app.setMainWidget(&fm);
    fm.setCaption("QProgressDialog element & stress test");
    fm.show();
    return app.exec();
}
