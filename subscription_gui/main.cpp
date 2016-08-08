#include <QApplication>
#include "mainwindow.h"
#include "task/task.h"

#include <iostream>

int main(int argc, char *argv[])
{
    qRegisterMetaType<SubscriptionType>("Subscription");
    qRegisterMetaType<Task*>("Task*");

    QApplication a(argc, argv);

    MainWindow mainWin;
    mainWin.show();

    return a.exec();
}
