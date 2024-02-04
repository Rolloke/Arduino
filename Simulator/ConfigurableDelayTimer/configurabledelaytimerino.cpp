#include <Arduino.h>
#include <unittest.h>

#include "../../../Arduino/Sketches/ConfigurableDelayTimer/ConfigurableDelayTimer.ino"
#include "ui_mainwindow.h"

void initialize(Ui_MainWindow* ui)
{
    (void) ui;
    // ui->labelHelp->setText(QObject::tr("Press %1 to get help").arg(ui->btnSendText->text()));
    ui->textToSendToSerialDevice->setText("help");
}

void doTest()
{
    // test simple input output binary
    gMainWindowPointer->sendSerialText("input[1]:pin[8]:binary");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("output[1]:pin[9]:binary");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("input[1]:connect:output[1]");
    ARDUINO_LOOP
    gMainWindowPointer->setPinValue(8, 0);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(9), 0);
    gMainWindowPointer->setPinValue(8, 1);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(9), 1);


    // test simple input output analog
    gMainWindowPointer->sendSerialText("input[2]:pin[10]:analog");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("output[2]:pin[11]:analog");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("input[2]:connect:output[2]");
    ARDUINO_LOOP
    gMainWindowPointer->setPinValue(10, 0);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(11), 0);
    gMainWindowPointer->setPinValue(10, 1000);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(11), 250);

    // switch on of analog output with digital input
    gMainWindowPointer->sendSerialText("input[3]:pin[7]:binary");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("output[3]:pin[6]:analog");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("input[3]:connect:output[3]");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("output[3]:value:200");
    ARDUINO_LOOP
    gMainWindowPointer->setPinValue(7, 1);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 200);
    gMainWindowPointer->setPinValue(7, 0);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 0);
    gMainWindowPointer->setPinValue(7, 1);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 200);

    // change output 3 to tone
    gMainWindowPointer->sendSerialText("output[3]:pin[6]:tone");
    ARDUINO_LOOP
    // set annother frequency
    gMainWindowPointer->sendSerialText("output[3]:value:5000");
    ARDUINO_LOOP

    // switch on of tone output
    gMainWindowPointer->setPinValue(7, 1);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 5000);
    gMainWindowPointer->setPinValue(7, 0);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 0);
    gMainWindowPointer->setPinValue(7, 1);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(6), 5000);


    // switch on of analog output with digital input
    gMainWindowPointer->sendSerialText("input[4]:pin[A0]:analog");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("input[4]:threshold:100:5");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("output[4]:pin[13]:binary");
    ARDUINO_LOOP
    gMainWindowPointer->sendSerialText("input[4]:connect:output[4]");
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(13), 0);
    gMainWindowPointer->setPinValue(A0, 100);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(13), 0);

    gMainWindowPointer->setPinValue(A0, 106);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(13), 1);

    gMainWindowPointer->setPinValue(A0, 95);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(13), 1);

    gMainWindowPointer->setPinValue(A0, 94);
    ARDUINO_LOOP
    TEST_CHECK_EQUAL(gMainWindowPointer->getPinValue(13), 0);
}
