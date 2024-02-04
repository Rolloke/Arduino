#include "logicview.h"
#include <algorithm>
#include <cmath>
#include <QMoveEvent>
// TODO: show analog curve and validate

LogicView::LogicView(QWidget*parent) :
    QGraphicsView(parent)
  , mGridLinesPen(QColor(Qt::darkGreen))
  , mCurvesPen(QColor(Qt::red))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(QBrush(QColor(Qt::lightGray), Qt::SolidPattern));
    mGridLinesPen.setStyle(Qt::DotLine);
    mGridLinesPen.setWidth(0);
    mCurvesPen.setWidth(0);
}

void LogicView::fitRangeInView(QRectF& range)
{
    QRectF view = frameRect();
    if (mSelectedRange)
    {
        range = mSelectedRange.get();
    }

    resetTransform();
    translate(-range.left(), -range.top());
    scale(view.width() / range.width(), view.height() / range.height());
    translate(view.left(), view.top());
    setSceneRect(range);
}

void LogicView::determineRange(const PinMap& aPins)
{
    QRectF range;

    PinMap::const_iterator fMin = std::min_element(aPins.begin(), aPins.end(),  [] (PinMap::const_reference aP1, PinMap::const_reference aP2 )
    {
        return aP1.second.begin()->first < aP2.second.begin()->first;
    });

    PinMap::const_iterator fMax = std::max_element(aPins.begin(), aPins.end(),  [] (PinMap::const_reference aP1, PinMap::const_reference aP2 )
    {
        return aP1.second.rbegin()->first < aP2.second.rbegin()->first;
    });

    if (fMin != aPins.end() && fMax != aPins.end())
    {
        range.setLeft(fMin->second.begin()->first);
        range.setRight(fMax->second.rbegin()->first);
        float fEnhance = range.width() * 0.05;
        range.setLeft(range.left());//   - fEnhance);
        if (mCurrentRecordTime)
        {
            range.setRight(*mCurrentRecordTime);
        }
        else
        {
            range.setRight(range.right() + fEnhance);
        }
    }
    else
    {
        range.setLeft(0);
        range.setRight(7);
    }

    range.setTop(aPins.size()+0.1);
    range.setBottom(-0.1);

    mRange = range;

    fitRangeInView(range);
}

void LogicView::clearSelectedRange()
{
    mSelectedRange.reset();
    mCurrentRecordTime.reset();
    Q_EMIT sendUpdate();
}

void LogicView::setCurrentRecordTime(float aTime_ms)
{
    if (aTime_ms > 0)
    {
        mCurrentRecordTime = aTime_ms;
    }
    else
    {
        mCurrentRecordTime.reset();
    }
}

namespace
{
const float fHeight = 0.8f;
const float fOffset = 0.1f;
}

void LogicView::drawLogicCurves(const PinMap& aPins)
{
    scene()->clear();

    float       fLevel  = 0.0f;
    scene()->addLine(mRange.left(), fLevel, mRange.right(), fLevel, mGridLinesPen);

    for (auto& fPin : aPins)
    {
        PolyLineItem*pPolyLineItem = new PolyLineItem;
        QPolygonF& fPolyLine = pPolyLineItem->getPolyLine();
        pPolyLineItem->setPen(&mCurvesPen);

        auto& fPoints = fPin.second;
        if (fPoints.size())
        {
            auto fPi = fPoints.begin();

            float fValue = fLevel + (fOffset + fPi->second * fHeight);
            if (__isnan(fValue) ) fValue = 0;
            fPolyLine.append(QPointF(mRange.left(), fValue));
            for (++fPi; fPi != fPoints.end(); ++fPi)
            {
                fPolyLine.append(QPointF(fPi->first, fValue));
                fValue = fLevel + (fOffset + fPi->second * fHeight);
                fPolyLine.append(QPointF(fPi->first, fValue));
            }
            fPolyLine.append(QPointF(mRange.right(), fValue));
        }

        scene()->addItem(pPolyLineItem);

        fLevel += 1.0;
        scene()->addLine(mRange.left(), fLevel, mRange.right(), fLevel, mGridLinesPen);
    }
}

float LogicView::getCurveLevel(float aLevelY)
{
    float fLevel = aLevelY - static_cast<int>(aLevelY);
    fLevel /= fHeight;
    fLevel -= fOffset;
    return fLevel;
}

void LogicView::mouseDoubleClickEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseDoubleClickEvent(event);
    clearSelectedRange();
}


void LogicView::mousePressEvent ( QMouseEvent * event )
{
    QGraphicsView::mousePressEvent(event);

    mButtonsWerePressed = event->buttons();
    mSelectionStartPoint = event->pos();
}

void LogicView::wheelEvent(QWheelEvent *event)
{
    if (mSelectedRange)
    {
        const float fDeltaM = 140;
        float fFactor = 0;
        if (event->delta() > 0)
        {
            fFactor = event->delta() / fDeltaM;
        }
        else
        {
            fFactor = -fDeltaM / event->delta();
        }
        QPointF fAngleDelta = mapToScene(event->pos());
        QRectF& fRect = *mSelectedRange;
        float fNewHalfWidth = fRect.width() * fFactor * 0.5;
        fRect.setLeft(fAngleDelta.x() - fNewHalfWidth);
        fRect.setRight(fAngleDelta.x() + fNewHalfWidth);
        Q_EMIT sendUpdate();
    }
}

void LogicView::mouseReleaseEvent ( QMouseEvent * event )
{
    QGraphicsView::mouseReleaseEvent(event);
    if (dragMode() == RubberBandDrag && mSelectionStartPoint)
    {
        QRect fSelection = QRect(mSelectionStartPoint.get(), event->pos()).normalized();
        if (mButtonsWerePressed & Qt::LeftButton)
        {
            if (fSelection.width() > 3 && fSelection.height() > 3)
            {
                QRectF fSceneRect(mapToScene(fSelection.topLeft()), mapToScene(fSelection.bottomRight()));
                mSelectedRange = mRange;
                mSelectedRange->setLeft(fSceneRect.left());
                mSelectedRange->setRight(fSceneRect.right());
                Q_EMIT sendUpdate();
            }
        }
        else if (mButtonsWerePressed & Qt::RightButton)
        {
            if (fSelection.width() > 3 && fSelection.height() > 3)
            {
                QRectF fSceneRect(mapToScene(fSelection.topLeft()), mapToScene(fSelection.bottomRight()));
                QString fString = "Width: " + QString::number(fSceneRect.width() * 0.001) + " s, Height: " + QString::number(fSceneRect.height() / fHeight) + " V";
                Q_EMIT(setStatusText(fString));
            }
            else
            {
                QPointF fPoint = mapToScene(event->pos());
                QString fString = "Position: " + QString::number(fPoint.x()*0.001) + " s, " + QString::number(getCurveLevel(fPoint.y())) + " V";
                Q_EMIT(setStatusText(fString));
            }
        }
        mSelectionStartPoint.reset();
    }
}


PolyLineItem::PolyLineItem() :
    mpPen(0)
{

}

PolyLineItem::~PolyLineItem()
{

}

QRectF PolyLineItem::boundingRect() const
{
    return mPolyLine.boundingRect();
}

void PolyLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    if (mpPen) painter->setPen(*mpPen);
    painter->drawPolyline(mPolyLine);
}
