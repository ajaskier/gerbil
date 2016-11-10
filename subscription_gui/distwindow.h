#ifndef DISTWINDOW_H
#define DISTWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

namespace Ui {
class DistWindow;
}

class DistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DistWindow(QWidget *parent = 0);
    ~DistWindow();

private:
    Ui::DistWindow *ui;
    Subscription* distSub;

signals:
    void calculateRequest();

private slots:
    void displayDist();
    void on_pushButton_clicked();
};

#endif // DISTWINDOW_H
