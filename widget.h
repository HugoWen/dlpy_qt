#ifndef WIDGET_H
#define WIDGET_H

#include "thread.h"
#include <QWidget>
#include <QCloseEvent>


namespace Ui {
class widget;
}

class widget : public QWidget
{
    Q_OBJECT
    
public:
    widget(QWidget *parent = 0);
    ~widget();
    
protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

private:
    Ui::widget *ui;
    Thread *th;

private slots:
    void on_pushButton_in_clicked();
    void ReadData(QByteArray temp);
};

#endif // WIDGET_H
