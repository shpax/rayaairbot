#include "quadrotorjoystick.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QTimer>
#include <map>


QuadrotorJoystick::~QuadrotorJoystick()
{
    delete ui;
}

void QuadrotorJoystick::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QuadrotorJoystick::QuadrotorJoystick(int joy_dev_, QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
    emergenciaSTOP=1;
    ui->setupUi(this);

    joy_dev = joy_dev_;

    joy = new QJoystick();

    QStringList joylist = joy->joyList();
    for(int i=0; i<joylist.size();i++) {
        qWarning("Joystick %d, %s\n", i, joylist.at(i).toAscii().data() );
    }

    joy->open(joy_dev);

    connect(joy, SIGNAL(axisValueChanged(int,int)), this, SLOT(axisValueChanged(int,int)));
    connect(joy, SIGNAL(buttonValueChanged(int,bool)), this, SLOT(buttonValueChanged(int,bool)));

    //QTimer::singleShot(100, this, SLOT(initValues()));

    tcp =  new Tcp;

    connect(tcp, SIGNAL(newCommand(quint8,QString)), this, SLOT(newCommandProcess(quint8,QString)));



}

void QuadrotorJoystick::buttonValueChanged(int boton, bool b) {
    if(b) {
        if(boton==0) {
            on_pushButton_clicked();
        } else if(boton==1) {
            initValues();
        }
    }
}

void QuadrotorJoystick::initValues(void) {    
    on_spinBox_P_valueChanged(ui->spinBox_P->value());
    on_spinBox_I_valueChanged(ui->spinBox_I->value());
    on_spinBox_D_valueChanged(ui->spinBox_D->value());
    on_spinBox_I_Max_valueChanged(ui->spinBox_I_Max->value());

    on_spinBox_P_2_valueChanged(ui->spinBox_P_2->value());
    on_spinBox_I_2_valueChanged(ui->spinBox_I_2->value());
    on_spinBox_D_2_valueChanged(ui->spinBox_D_2->value());
    on_spinBox_I_Max_2_valueChanged(ui->spinBox_I_Max_2->value());
}


void QuadrotorJoystick::newCommandProcess(quint8 comando, QString s) {
    QList<QString> dato;
    DataPlot *dataplot;
    DataPlot2 *dataplot2;

    switch(comando) {
    case 's':
        //        qWarning() << dato[0].toInt();
        //        qWarning() << "serial: " << s;

        dato = s.split(' ');

        if(dato.size() == 8) {
            dataplot = qobject_cast<DataPlot *>(ui->widget_plot);
            if (dataplot) {
                ui->widget_plot->addData(dato[0].toInt(), dato[1].toInt());
//                ui->widget_plot->addData(dato[0].toInt(), dato[1].toInt());
            }
            dataplot2 = qobject_cast<DataPlot2 *>(ui->widget_plot_2);
            if (dataplot2) {
//                ui->widget_plot_2->addData(dato[2].toInt(), dato[3].toInt(), dato[4].toInt());
                ui->widget_plot_2->addData(dato[2].toInt(), dato[4].toInt(), dato[7].toInt());
            }
            ui->label_angulo1->setText(dato[5]);
            ui->label_angulo2->setText(dato[7]);
        } else {
            qDebug() << "s: " << s;
        }

        break;
    case 'l':
        qDebug() << "l" << s;
        break;
    default: break;
    }
}

void QuadrotorJoystick::axisValueChanged(int axis, int value) {
    QString s;
//    qWarning("axis: %d value: %d", axis, value);

    s = "c";
    s += axis;
    value = (int)(((float)value)*1000.0/32676.0);

    switch(axis) {
    case 0:
        value = value/20;
        break;
    case 1:
        value = -1*value/20;
        break;
    case 2:
        value = value;
        break;
    case 3:
        value = -value;
        break;
    }

    s += (value>>8)&0x00FF;
    s += value&0x00FF;

//    qWarning("enviando: axis: %d value1: %u value2: %u value: %d", axis, (value>>8)&0x00FF, value&0x00FF, value);
    qWarning("enviando: axis: %d value: %d", axis, value);
    serialTX(s);
    if(value==0) // reenviando por si no llega bien el 0 que es mas importante que el resto
        serialTX(s);

}

void QuadrotorJoystick::on_spinBox_P_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_P->setValue(value);
    s = "s";
    s += (char)1;
    s += "P";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: P0: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_P_valueChanged(int value)
{
    ui->spinBox_P->setValue(value);
}

void QuadrotorJoystick::on_spinBox_I_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_I->setValue(value);
    s = "s";
    s += (char)1;
    s += "I";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: I0: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_I_valueChanged(int value)
{
    ui->spinBox_I->setValue(value);

}

void QuadrotorJoystick::on_spinBox_D_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_D->setValue(value);
    s = "s";
    s += (char)1;
    s += "D";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: D0: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_D_valueChanged(int value)
{
    ui->spinBox_D->setValue(value);
}

void QuadrotorJoystick::on_spinBox_I_Max_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_I_Max->setValue(value);
    s = "s";
    s += (char)1;
    s += "W";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: M0: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_I_Max_valueChanged(int value)
{
    ui->spinBox_I_Max->setValue(value);
}

void QuadrotorJoystick::on_pushButton_clicked()
{
    if(emergenciaSTOP) {
        ui->pushButton->setText("Stop");
        ui->pushButton->setStyleSheet("background-color: rgb(255, 0, 0);");
        setEmergenciaSTOP(false);
    } else {
        ui->pushButton->setText("Start");
        ui->pushButton->setStyleSheet("background-color: rgb(0, 255, 0);");
        setEmergenciaSTOP(true);
    }
}

void QuadrotorJoystick::setEmergenciaSTOP(bool b)
{
    QString s;
    s += 'S';
    if(b) {
        qWarning("Stop");
        s += (char)1;
        serialTX(s);
        serialTX(s);

        emergenciaSTOP = 1;
    } else {
        qWarning("Start");
        s += (char)0;
        emergenciaSTOP = 0;
        serialTX(s);
        serialTX(s);

    }
}





void QuadrotorJoystick::on_spinBox_P_2_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_P_2->setValue(value);
    s = "s";
    s += (char)0;
    s += "P";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: P1: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_P_2_valueChanged(int value)
{
    ui->spinBox_P_2->setValue(value);
}

void QuadrotorJoystick::on_spinBox_I_2_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_I_2->setValue(value);
    s = "s";
    s += (char)0;
    s += "I";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: I1: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_I_2_valueChanged(int value)
{
    ui->spinBox_I_2->setValue(value);
}

void QuadrotorJoystick::on_spinBox_D_2_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_D_2->setValue(value);
    s = "s";
    s += (char)0;
    s += "D";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: D1: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_D_2_valueChanged(int value)
{
    ui->spinBox_D_2->setValue(value);
}

void QuadrotorJoystick::on_spinBox_I_Max_2_valueChanged(int value)
{
    QString s;
    ui->horizontalSlider_I_Max_2->setValue(value);
    s = "s";
    s += (char)0;
    s += "W";
    s += (value>>8)&0x00FF;
    s += value&0x00FF;
    qWarning("enviando: M1: X value: %d", value);
    serialTX(s);
}

void QuadrotorJoystick::on_horizontalSlider_I_Max_2_valueChanged(int value)
{
    ui->spinBox_I_Max_2->setValue(value);
}

void QuadrotorJoystick::on_pushButton_send_l_clicked()
{
    tcp->send('l', "Este es el comando l");

}

void QuadrotorJoystick::on_pushButton_send_s_clicked()
{
    tcp->send('s', "Este es el comando s");

}

void QuadrotorJoystick::serialTX(QString s) {
    tcp->send('s', s);

}
