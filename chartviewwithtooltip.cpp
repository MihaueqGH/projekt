#include "chartviewwithtooltip.h"
#include <QMouseEvent>
#include <QDateTime>
#include <cmath>

ChartViewWithTooltip::ChartViewWithTooltip(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
    tooltip = new QLabel(this);
    tooltip->setStyleSheet("QLabel { background-color: white; border: 1px solid gray; padding: 4px; }");
    tooltip->setVisible(false);
    tooltip->setAttribute(Qt::WA_TransparentForMouseEvents);
    setMouseTracking(true);
}

void ChartViewWithTooltip::mouseMoveEvent(QMouseEvent *event)
{
    QPoint mousePoint = event->pos();
    bool found = false;

    for (QAbstractSeries *series : chart()->series()) {
        QXYSeries *xySeries = qobject_cast<QXYSeries *>(series);
        if (!xySeries) continue;

        for (const QPointF &point : xySeries->points()) {
            QPointF scenePoint = chart()->mapToPosition(point, series);
            double distance = std::hypot(scenePoint.x() - mousePoint.x(), scenePoint.y() - mousePoint.y());

            if (distance < maxDistance) {
                QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x()));
                QString text = QString("Data: %1\nWartość: %2")
                                   .arg(dt.toString("yyyy-MM-dd HH:mm:ss"))
                                   .arg(point.y());

                tooltip->setText(text);
                tooltip->adjustSize();
                tooltip->move(mousePoint + QPoint(10, -tooltip->height() - 10));
                tooltip->setVisible(true);
                found = true;
                break;
            }
        }

        if (found) break;
    }

    if (!found) {
        tooltip->setVisible(false);
    }

    QChartView::mouseMoveEvent(event);
}

void ChartViewWithTooltip::leaveEvent(QEvent *event)
{
    tooltip->setVisible(false);
    QChartView::leaveEvent(event);
}
