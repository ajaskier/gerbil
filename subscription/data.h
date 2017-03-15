#ifndef DATA_H
#define DATA_H

#include <QDebug>

struct Data {
    Data()
    {
       // qDebug() << "data created";
    }

    Data(Data const& source) {
        num = source.num;
       // qDebug() << "copied";
    }

    ~Data() {
       // qDebug() << "data destroyed with" << num;
    }

    int num = -90;
};

#endif // DATA_H
