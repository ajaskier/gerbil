#ifndef BWINDOW_H
#define BWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

namespace Ui {
class BWindow;
}

class BWindow : public QWidget
{
	Q_OBJECT

public:
	explicit BWindow(QWidget *parent = 0);
	~BWindow();

private:
	Ui::BWindow *ui;
	Subscription* dataBSub;
	Subscription* dataDSub;


signals:
	void buttonBClicked();
	void buttonDClicked();
	void inputBChanged(int b);
	void inputDChanged(int d);

private slots:
	void displayB();
	void displayD();

	void on_computeBButton_clicked();
	void on_computeDButton_clicked();
	void on_inputD_textChanged(const QString &arg1);
	void on_inputB_textChanged(const QString &arg1);
};

#endif // BWINDOW_H
