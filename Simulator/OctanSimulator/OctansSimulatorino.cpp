#include <Arduino.h>

#include "../../Sketches/OctansSimulator/OctansSimulatorDisplay/OctansSimulatorDisplay.ino"
#include <ui_mainwindow.h>

void initialize(Ui_MainWindow *ui)
{
    (void)ui; /// NOTE: you may use ui Interface here

    if (ui->EditLeft->text().size() == 0)
    {
        ui->EditLeft->setText(QObject::tr("-%1").arg(BTN_BIT0));
    }
    if (ui->EditRight->text().size() == 0)
    {
        ui->EditRight->setText(QObject::tr("-%1").arg(BTN_BIT1));
    }
    if (ui->EditDown->text().size() == 0)
    {
        ui->EditDown->setText(QObject::tr("-%1").arg(BTN_BIT2));
    }
    if (ui->EditUp->text().size() == 0)
    {
        ui->EditUp->setText(QObject::tr("-%1,-%2").arg(BTN_BIT0).arg(BTN_BIT1));
    }
    if (ui->EditEsc->text().size() == 0)
    {
        ui->EditEsc->setText(QObject::tr("-%1,-%2").arg(BTN_BIT0).arg(BTN_BIT2));
    }
    if (ui->EditEnter->text().size() == 0)
    {
        ui->EditEnter->setText(QObject::tr("-%1,-%2").arg(BTN_BIT1).arg(BTN_BIT2));
    }
}

void doTest()
{
    /// NOTE: you may implement a unittest
}
