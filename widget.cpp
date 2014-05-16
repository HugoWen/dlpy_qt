#include "widget.h"
#include "ui_widget.h"

widget::widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget)
{
    setGeometry(0, 0, 800, 25);
    ui->setupUi(this);

    th = NULL;
}

widget::~widget()
{
    delete ui;
}

void widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void widget::on_pushButton_in_clicked()
{
   // QString text = ui->comboBox->currentText();
    th = new Thread("/dev/ttyACM0", this);
    th->start();
    connect(th, SIGNAL(serialFinished(QByteArray)), this, SLOT(ReadData(QByteArray)));

}

void widget::ReadData(QByteArray temp)
{
    ui->lineEdit_in->setText(temp);
    //ui->textBrowser->insertPlainText(tr("\n\n"));
}

void widget::closeEvent(QCloseEvent *event)
{
    if(th != NULL)
    {
        th->stopThread();
        th->wait();
    }
    event->accept();
}
