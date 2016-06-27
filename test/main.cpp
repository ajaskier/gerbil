#include <iostream>
#include <gmock/gmock.h>


#include "background_task/background_executor.h"
#include "simulation.h"

#include <QApplication>



int main(int argc, char **argv) {
//    testing::InitGoogleMock(&argc, argv);
//    return RUN_ALL_TESTS();

    QApplication app(argc, argv);

    Simulation flow;

    app.exec();

}
