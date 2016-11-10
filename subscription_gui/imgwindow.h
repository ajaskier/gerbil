#ifndef IMGWINDOW_H
#define IMGWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

namespace Ui {
class ImgWindow;
}

class ImgWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImgWindow(QWidget *parent = 0);
    ~ImgWindow();

private:
    Ui::ImgWindow *ui;
    Subscription* sub;

signals:
    void calculateRequest();

private slots:
    void display();
    void on_pushButton_clicked();
};

#endif // IMGWINDOW_H
