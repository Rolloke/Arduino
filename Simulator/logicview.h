#ifndef LOGICVIEW_H
#define LOGICVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

#include <map>
#include <unordered_map>
#include <string>
#include <boost/optional.hpp>

#define VIEW_DIRECTION 1

typedef std::map<int, float> PinValueMap;
typedef std::map<std::string, PinValueMap> PinMap;


class LogicView : public QGraphicsView
{
    Q_OBJECT
public:
    LogicView(QWidget *parent = 0);

    void determineRange(const PinMap& aPins);
    void drawLogicCurves(const PinMap& aPins);
    void clearSelectedRange();
    void setCurrentRecordTime(float aTime_ms);

Q_SIGNALS:
    void sendUpdate();
    void setStatusText(const QString& aText);

private:
    void fitRangeInView(QRectF& range);
    float getCurveLevel(float aLevelY);

    void	mouseDoubleClickEvent ( QMouseEvent * event ) override;
    void	mousePressEvent ( QMouseEvent * event ) override;
    void	mouseReleaseEvent ( QMouseEvent * event ) override;
    void    wheelEvent(QWheelEvent *event) override;

    boost::optional<QPoint> mSelectionStartPoint;
    Qt::MouseButtons        mButtonsWerePressed;
    boost::optional<QRectF> mSelectedRange;
    QPen mGridLinesPen;
    QPen mCurvesPen;

    QRectF mRange;
    boost::optional<float> mCurrentRecordTime;

};



class PolyLineItem : public QGraphicsItem
{
 public:
     PolyLineItem();
     virtual ~PolyLineItem();

     void setPen(const QPen* aPen) { mpPen = aPen; }
     QPolygonF& getPolyLine() { return mPolyLine; }

protected:
     QRectF boundingRect() const;
     void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
     QPolygonF mPolyLine;
     const QPen*     mpPen;
 };

#endif // LOGICVIEW_H
