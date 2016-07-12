#include "controller.h"
#include <QDebug>
#include <QThread>


Controller::Controller(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Controller created!";

}

void Controller::init()
{
    modelA = new ModelA(5, this);
    modelB = new ModelB(3, this);

    modelB->delegateTask();


}
