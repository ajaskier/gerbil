#ifndef AWINDOW_H
#define AWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

namespace Ui {
class AWindow;
}

class AWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AWindow(QWidget *parent = 0);
    ~AWindow();

private:
    Ui::AWindow *ui;
    Subscription* dataASub;

signals:
    void buttonAClicked();
    void inputAChanged(int text);


private slots:
    void displayA();

    void on_computeAButton_clicked();
    void on_inputA_textChanged(const QString &arg1);
};

#endif // AWINDOW_H
