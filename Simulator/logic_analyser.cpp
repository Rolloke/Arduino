#include "logic_analyser.h"
#include "ui_logic_analyser.h"
#include <logger.h>

#include <QShowEvent>
#include <QLabel>

// TODO: update view on size change

LogicAnalyser::LogicAnalyser(QWidget *parent) :
    QDialog(parent, Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::Window)
  , mUpdateTimerID(0)
  , ui(new Ui::LogicAnalyser)
{
    ui->setupUi(this);
    QGraphicsScene* scene = new QGraphicsScene;
    ui->graphicsViewLogicCurves->setScene(scene);

    connect(ui->graphicsViewLogicCurves, SIGNAL(sendUpdate()), this, SLOT(on_btnUpdate_clicked()));
    ui->graphicsViewLogicCurves->setDragMode(QGraphicsView::RubberBandDrag);

    connect(ui->graphicsViewLogicCurves, SIGNAL(setStatusText(QString)), ui->labelStatusbar, SLOT(setText(QString)));

    mPinMap.reset(new PinMap);
}

LogicAnalyser::~LogicAnalyser()
{
     delete ui;
}

bool LogicAnalyser::isRecording()
{
    return ui->btnRecord->isChecked();
}

void LogicAnalyser::setValue(const std::string &aPin, int aTime, float aValue)
{
    auto& fPinMap = *mPinMap;
    auto& fPin = fPinMap[aPin];
    if (fPin.size())
    {
        if (fPin.rbegin()->second != aValue)
        {
            fPin[aTime] = aValue;
            TRACE(Logger::info, "setValue(%s, %d, %f)", aPin.c_str(), aTime, aValue);
        }
    }
    else
    {
        fPin[aTime] = aValue;
    }
}

void LogicAnalyser::setVisible(bool visible)
{
    QDialog::setVisible(visible);
    Q_EMIT(isshown(visible));
    if (visible)
    {
        on_btnUpdate_clicked();
    }
}


void LogicAnalyser::timerEvent(QTimerEvent *aId)
{
    if (aId->timerId() == mUpdateTimerID)
    {
        ui->graphicsViewLogicCurves->setCurrentRecordTime(millis());
        on_btnUpdate_clicked();
    }
}

void LogicAnalyser::resizeEvent(QResizeEvent *aEvt)
{
    QDialog::resizeEvent(aEvt);
    on_btnUpdate_clicked();
}

void LogicAnalyser::on_btnClear_clicked()
{
    mPinMap->clear();
    ui->graphicsViewLogicCurves->clearSelectedRange();
}

void LogicAnalyser::on_btnRecord_clicked(bool checked)
{
    if (checked)
    {
        on_ckAutoUpdate_clicked(ui->ckAutoUpdate->isChecked());
    }
    else
    {
        on_ckAutoUpdate_clicked(false);
        on_btnUpdate_clicked();
    }
    Q_EMIT(record(checked));
}

void LogicAnalyser::on_ckAutoUpdate_clicked(bool checked)
{
    if (checked)
    {
        if (!mUpdateTimerID)
        {
            mUpdateTimerID = startTimer(1000);
        }
    }
    else
    {
        if (mUpdateTimerID)
        {
            killTimer(mUpdateTimerID);
            mUpdateTimerID = 0;
        }
    }
}

void LogicAnalyser::updateVerticalLayoutItems()
{
    const int fWidgets = ui->verticalLayoutNames->count();
    int fWidget = 0;

    for (auto fPin = mPinMap->rbegin(); fPin != mPinMap->rend(); ++fPin)
    {
        const auto& fText   = fPin->first;
        if (fWidget < fWidgets)
        {
            QLabel *fItem = dynamic_cast<QLabel*>(ui->verticalLayoutNames->itemAt(fWidget)->widget());
            if (fItem)
            {
                fItem->setText(fText.c_str());
                fItem->show();
            }
        }
        else
        {
            ui->verticalLayoutNames->addWidget(new QLabel(QString(fText.c_str()), this));
        }
        ++fWidget;
    }

    for ( ; fWidget < fWidgets; ++fWidget)
    {
        ui->verticalLayoutNames->itemAt(fWidget)->widget()->hide();
    }
}

void LogicAnalyser::on_btnUpdate_clicked()
{
    updateVerticalLayoutItems();
    ui->graphicsViewLogicCurves->determineRange(*mPinMap);
    ui->graphicsViewLogicCurves->drawLogicCurves(*mPinMap);
}


