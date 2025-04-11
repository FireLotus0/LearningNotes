#include "app/app.h"

#include "utils/task/taskexecutor.h"
#include <qapplication.h>

int main(int argc, char** argv) {
    TASK_EXECUTOR.start(4);
    QApplication a(argc, argv);

    App app;
    app.show();

    return a.exec();
}
