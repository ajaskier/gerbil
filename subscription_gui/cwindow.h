#ifndef CWINDOW_H
#define CWINDOW_H

#include <QWidget>
#include <subscription_manager.h>

namespace Ui {
class CWindow;
}

class CWindow : public QWidget
{
	Q_OBJECT

public:
	explicit CWindow(QWidget *parent = 0);
	~CWindow();

private:
	Ui::CWindow *ui;
	Subscription* dataCSub;

signals:
	void buttonCClicked();
	void inputCChanged(int c);


private slots:
	void displayC();

	void on_computeCButton_clicked();
	void on_inputC_textChanged(const QString &arg1);
};

#endif // CWINDOW_H
