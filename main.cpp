#include "isplcodegen.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IsplCodeGen w;
    w.show();
    return a.exec();
}
