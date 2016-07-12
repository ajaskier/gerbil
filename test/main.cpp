#include <QCoreApplication>
#include <controller.h>

#include <iostream>

int main(int argc, char *argv[]) {

    qRegisterMetaType<DataId>("DataId");

    QCoreApplication a(argc, argv);
    Controller c(&a);
    c.init();

    return a.exec();

}
