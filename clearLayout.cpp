#include "clearLayout.h"
#include <QWidget>
#include <QLayoutItem>

void ClearLayout::clearLayout(QLayout *layout, QLabel *mainInformation) {
    if (!layout) return;

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->hide();
            widget->deleteLater();
        }

        if (QLayout *childLayout = item->layout()) {
            clearLayout(childLayout, mainInformation);
        }

        delete item;
    }

    // Ustawiamy tekst na QLabel
    mainInformation->setText("Dane pogodowe - Projekt");
}
