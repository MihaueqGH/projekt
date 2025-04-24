#ifndef CHARTVIEWWITHTOOLTIP_H
#define CHARTVIEWWITHTOOLTIP_H

#pragma once

#include <QtCharts>
#include <QLabel>

class ChartViewWithTooltip : public QChartView {
    Q_OBJECT

public:
    explicit ChartViewWithTooltip(QChart *chart, QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QLabel *tooltip;
    const double maxDistance = 20.0;  // Maksymalna odległość w pikselach od punktu, żeby pokazać dymek
};




#endif // CHARTVIEWWITHTOOLTIP_H
