#include "stationtable.h"
#include <QJsonObject>
#include <QHeaderView>
#include "api.h"
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QTimer>
#include <QDir>
#include <QtCharts>
#include <QChartView>
#include <QLineSeries>
#include <QComboBox>>
/**
 * @brief Konstruktor klasy StationTable.
 *
 * Tworzy i inicjalizuje widok tabeli stacji wraz z polem filtrowania.
 * Podłącza sygnały do obsługi kliknięć oraz filtrowania stacji.
 *
 * @param parentLayout Układ, do którego dodawane są widżety.
 */
StationTable::StationTable(QVBoxLayout *parentLayout) {
    filterEdit = new QLineEdit();
    filterEdit->setPlaceholderText("Filtruj stacje");
    filterEdit->setFixedHeight(30);

    table = new QTableWidget();
    table->setColumnCount(1);
    table->setHorizontalHeaderLabels({"Stacje"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSortingEnabled(false);
    table->setStyleSheet(R"(
    QTableWidget::item:hover {
        background-color: rgba(255,255,255,100);
    }
)");
    table->horizontalHeader()->setStretchLastSection(true);

    parentLayout->addWidget(filterEdit);
    parentLayout->addWidget(table);

    this->parentLayout = parentLayout;

    QObject::connect(table, &QTableWidget::cellClicked, this, &StationTable::onCellClicked);
    QObject::connect(filterEdit, &QLineEdit::textChanged, [this](const QString &text){
        filterTable(text);
    });
}
/**
 * @brief Destruktor klasy StationTable.
 *
 * Usuwa dynamicznie alokowane widżety.
 */
StationTable::~StationTable() {
    delete table;
    delete filterEdit;
}
/**
 * @brief Wyświetla dane o stacjach w tabeli.
 *
 * Dane są przekazywane w formie tablicy JSON i wyświetlane w tabeli.
 *
 * @param data Tablica QJsonArray zawierająca dane stacji.
 */
void StationTable::showData(const QJsonArray &data) {
    table->clearContents();
    table->setRowCount(data.size());
    this->data = data;

    for (int i = 0; i < data.size(); ++i) {
        QJsonObject obj = data[i].toObject();
        QString stationId = QString::number(obj["id"].toInt());
        QString stationName = obj["stationName"].toString();
        QString province = obj["city"].toObject()["commune"].toObject()["provinceName"].toString();

        QString rowText = QString("Województwo: %0    |    Miejscowość: %1")
                              .arg(province)
                              .arg(stationName);

        table->setItem(i, 0, new QTableWidgetItem(rowText));
        table->setRowHeight(i, 35);
    }

    table->resizeColumnsToContents();
}
/**
 * @brief Zwraca wskaźnik do widgetu tabeli.
 *
 * @return Wskaźnik do QTableWidget używanego przez klasę.
 */
QTableWidget* StationTable::getTableWidget() {
    return table;
}
/**
 * @brief Filtrowanie tabeli na podstawie wpisanego tekstu.
 *
 * Ukrywa wiersze, które nie zawierają dopasowania do tekstu filtrowania.
 *
 * @param filterText Tekst używany do filtrowania zawartości tabeli.
 */
void StationTable::filterTable(const QString &filterText) {
    for (int row = 0; row < table->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item && item->text().contains(filterText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        table->setRowHidden(row, !match);
    }
}
/**
 * @brief Obsługa kliknięcia komórki tabeli.
 *
 * Po kliknięciu wiersza pobierane są dane czujników danej stacji.
 *
 * @param row Indeks klikniętego wiersza.
 */
void StationTable::onCellClicked(int row) {
    QJsonObject selectedStation = data[row].toObject();
    int stationId = selectedStation["id"].toInt();

    if (stationId == 0) {
        qDebug() << "Błąd: niepoprawne stationId!";
        return;
    }

    fetchApi(stationId, selectedStation);
}
/**
 * @brief Pobiera dane z API na podstawie podanego URL.
 *
 * Wysyła żądanie HTTP do API, otrzymuje dane w formacie JSON i przekazuje je do
 * funkcji obsługującej wyświetlanie danych.
 *
 * @param url URL API do pobrania danych.
 */
void StationTable::fetchApi(int stationId, const QJsonObject &stationData) {
    QString id = QString::number(stationId);
    QString url = QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1").arg(id);

    ApiManager::fetchData(url, [this, stationData](const QJsonArray &data) {
        if(error) {
            error = false;
            delete errorLabel;
        }

        for (int i = parentLayout->count() - 1; i >= 0; --i) {
            QWidget *widget = parentLayout->itemAt(i)->widget();
            if (widget && (qobject_cast<QChartView*>(widget) || qobject_cast<QPushButton*>(widget) || qobject_cast<QLabel*>(widget))) {
                delete widget;
            }
        }

        QString cityName = stationData["stationName"].toString();
        QLabel *locationLabel = new QLabel(cityName);
        locationLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px;");
        parentLayout->addWidget(locationLabel);
        locationLabel->setAlignment(Qt::AlignCenter);

        for (const QJsonValue &value : data) {
            QJsonObject paramObj = value.toObject();
            QJsonObject param = paramObj["param"].toObject();
            QString paramName = param["paramName"].toString();
            QString paramFormula = param["paramFormula"].toString();
            QString buttonText = QString("Pobierz dane: %1 (%2)").arg(paramName, paramFormula);
            QPushButton *paramButton = new QPushButton(buttonText);
            paramButton->setFixedHeight(30);
            parentLayout->addWidget(paramButton);

            int sensorId = paramObj["id"].toInt();

            connect(paramButton, &QPushButton::clicked, this, [this, stationData, sensorId, paramName, paramFormula]() {
                fetchSensorData(sensorId, stationData, paramName, paramFormula);
            });
        }
    },
                          [this](int errorCode) {
                              if (!error) {
                                  errorLabel = new QLabel("Błąd połączenia");
                                  errorLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: red; padding: 20px;");
                                  parentLayout->addWidget(errorLabel);
                                  errorLabel->setAlignment(Qt::AlignCenter);
                                  error = true;
                              }
                          });
}
/**
 * @brief Tworzy nowe okno wykresu i wyświetla dane na wykresie.
 *
 * Przyjmuje dane wejściowe i generuje wykres na podstawie tych danych. Okno wykresu jest
 * tworzone dynamicznie, a użytkownik może je zamknąć po zakończeniu oglądania.
 *
 * @param chartData Dane do wyświetlenia na wykresie.
 */
void StationTable::createChartWindow(QChartView *chartView, const QString &title) {
    if (chartWindow) {
        // Jeśli okno wykresu już istnieje, usuń je przed otwarciem nowego
        chartWindow->close();
        delete chartWindow;
    }

    // Tworzymy nowe okno wykresu
    chartWindow = new QMainWindow();
    chartWindow->setWindowTitle(title);
    chartWindow->setCentralWidget(chartView);
    chartWindow->resize(800, 600);
    chartWindow->show();
}
/**
 * @brief Sortuje dane stacji na podstawie daty.
 *
 * Sortuje dane po dacie w porządku rosnącym lub malejącym w zależności od argumentu.
 *
 * @param ascending Jeśli true, dane będą posortowane rosnąco. W przeciwnym razie malejąco.
 */
void StationTable::sortDataByDate(QJsonArray &valuesArray) {
    // Tworzymy listę z QJsonValue
    QList<QJsonValue> list;
    for (const QJsonValue &value : valuesArray) {
        list.append(value);
    }

    // Sortujemy tę listę za pomocą lambdy
    std::sort(list.begin(), list.end(), [](const QJsonValue &a, const QJsonValue &b) {
        QString dateStringA = a.toObject()["date"].toString();
        QString dateStringB = b.toObject()["date"].toString();
        QDateTime dateA = QDateTime::fromString(dateStringA, "yyyy-MM-dd hh:mm:ss");
        QDateTime dateB = QDateTime::fromString(dateStringB, "yyyy-MM-dd hh:mm:ss");

        // Zwracamy porównanie dat
        return dateA < dateB;
    });

    // Tworzymy nowy QJsonArray z posortowanej listy
    QJsonArray sortedArray;
    for (const QJsonValue &value : list) {
        sortedArray.append(value);
    }

    // Ustawiamy posortowaną tablicę
    valuesArray = sortedArray;
}

/**
 * @brief Pobiera dane dotyczące czujników z API dla danej stacji.
 *
 * Używa identyfikatora stacji do pobrania szczegółowych danych czujników stacji z API.
 *
 * @param stationId Identyfikator stacji pomiarowej.
 */
void StationTable::fetchSensorData(int sensorId, const QJsonObject &stationData, const QString &paramName, const QString &paramFormula) {
    QString url = QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(sensorId);

    ApiManager::fetchDataRaw(url, [this, sensorId, stationData, paramName, paramFormula](const QJsonDocument &doc) {
        // Usuń stare wykresy przed wyświetleniem nowego wykresu
        if (error == true) {
            error = false;
            delete errorLabel;
        }
        QLayout *layout = parentLayout->parentWidget()->layout();
        if (layout) {
            for (int i = layout->count() - 1; i >= 0; --i) {
                QWidget *widget = layout->itemAt(i)->widget();
                if (widget && qobject_cast<QChartView*>(widget)) {
                    delete widget;  // Usuwamy wykresy
                    break;  // Usuwamy tylko jeden wykres, ponieważ chcemy usunąć tylko istniejący wykres
                }
            }
        }

        // Przygotowanie danych z odpowiedzi API
        QJsonObject dataObj = doc.object();
        QString stationName = stationData["stationName"].toString();
        dataObj.insert("stationName", stationName);
        qDebug() << "pobieranie dla : " << stationName << "sensor: " << sensorId;
        if (!paramName.isEmpty()) {
            dataObj.insert("paramName", paramName);
        }

        // Dodanie klucza "sensorId" do obiektu danych
        QJsonObject dataWithSensorId = dataObj;  // Tworzymy kopię istniejącego obiektu danych
        dataWithSensorId.insert("sensorId", sensorId);  // Dodajemy klucz "sensorId" z wartością {sensorId}

        // Zapis do pliku JSON
        QString saveDir = QDir::currentPath() + "/data";
        QDir dir;
        if (!dir.exists(saveDir)) {
            dir.mkpath(saveDir);
        }

        // Zapis do pliku JSON
        QString fileName = QString("data_%1.json").arg(sensorId);
        QFile file(saveDir + "/" + fileName);

        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
            QJsonArray existingValues = loadDoc.object()["values"].toArray();

            // Usunięcie duplikatów na podstawie daty
            QSet<QString> seenDates;
            QJsonArray uniqueValues;
            for (const QJsonValue &value : existingValues) {
                QString date = value.toObject()["date"].toString();
                if (!seenDates.contains(date)) {
                    seenDates.insert(date);
                    uniqueValues.append(value);
                }
            }

            // Dodajemy nowe dane do istniejących
            QJsonArray newValues = dataObj["values"].toArray();
            for (const QJsonValue &newValue : newValues) {
                QString date = newValue.toObject()["date"].toString();
                if (!seenDates.contains(date)) {
                    seenDates.insert(date);
                    uniqueValues.append(newValue);
                }
            }

            // Filtrujemy dane po zakresie dat (przyjmujemy daty w formacie "yyyy-MM-dd hh:mm:ss")
            QString startDateStr = "2017-01-01 00:00:00"; // More inclusive date range
            QString endDateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"); // Up to current date
            QDateTime startDate = QDateTime::fromString(startDateStr, "yyyy-MM-dd HH:mm:ss");
            QDateTime endDate = QDateTime::fromString(endDateStr, "yyyy-MM-dd HH:mm:ss");

            QJsonArray filteredValues;
            for (const QJsonValue &value : uniqueValues) {
                QString dateString = value.toObject()["date"].toString();
                QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-dd HH:mm:ss");

                if (date >= startDate && date <= endDate) {
                    filteredValues.append(value);
                }
            }

            // Sortowanie po dacie
            sortDataByDate(filteredValues);

            // Aktualizujemy wartości w obiekcie danych
            dataWithSensorId.insert("values", filteredValues);

            // Zapisz do pliku
            QJsonDocument saveDoc(dataWithSensorId);
            file.resize(0);  // Wyczyść plik przed zapisaniem
            file.write(saveDoc.toJson());
            file.close();
        } else {
            QLabel *errorSaveLabel = new QLabel("Nie udało się zapisać danych do pliku.");
            errorSaveLabel->setStyleSheet("font-size: 12px; color: red; padding: 5px;");
            parentLayout->addWidget(errorSaveLabel);

            // Ukryj/usuń etykietę błędu po 2 sekundach (opcjonalnie)
            QTimer::singleShot(2000, errorSaveLabel, [errorSaveLabel]() {
                errorSaveLabel->deleteLater();
            });
        }

        QJsonArray valuesArray = dataObj["values"].toArray();
        QVector<QDateTime> dates;
        QVector<double> values;

        // Tworzymy scatterSeries i przechowujemy dane
        QScatterSeries *scatterSeries = new QScatterSeries();
        scatterSeries->setMarkerSize(8);

        // Tworzymy lineSeries do rysowania linii łączącej punkty
        QLineSeries *lineSeries = new QLineSeries();

        // Zmienne do analizy statystycznej
        double minValue = std::numeric_limits<double>::max();
        double maxValue = std::numeric_limits<double>::lowest();
        double sumValues = 0.0;
        int validCount = 0;

        for (const QJsonValue &value : valuesArray) {
            QJsonObject entry = value.toObject();
            QString dateString = entry["date"].toString();
            double valueData = entry["value"].toDouble();

            if (valueData == 0.0 || valueData == NULL) {
                continue; // Pomijamy ten punkt, jeśli wartość jest null lub 0
            }

            // Aktualizujemy statystyki
            minValue = qMin(minValue, valueData);
            maxValue = qMax(maxValue, valueData);
            sumValues += valueData;
            validCount++;

            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-dd hh:mm:ss");

            dates.append(date);
            values.append(valueData);

            scatterSeries->append(date.toMSecsSinceEpoch(), valueData);  // Dodajemy punkty do scatterSeries
            lineSeries->append(date.toMSecsSinceEpoch(), valueData);      // Dodajemy punkty do lineSeries (do rysowania linii)
        }

        // Obliczamy średnią jeśli mamy jakieś dane
        double avgValue = (validCount > 0) ? sumValues / validCount : 0.0;

        // Sprawdzamy, czy mamy jakieś dane
        bool hasData = (validCount > 0);

        // Tworzenie głównego widżetu i layoutu
        QWidget *mainWidget = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

        // Dodajemy tytuł stacji i parametru
        QLabel *titleLabel = new QLabel(QString("%1 - %2").arg(stationName).arg(paramName));
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; padding: 5px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        // Tworzymy widżet do organizacji wykresu i statystyk obok siebie
        QWidget *chartStatContainer = new QWidget();
        QHBoxLayout *chartStatLayout = new QHBoxLayout(chartStatContainer);

        // Tworzenie wykresu
        QChart *chart = new QChart();
        chart->addSeries(scatterSeries); // Dodajemy serie scatter
        chart->addSeries(lineSeries);    // Dodajemy serie linii
        chart->setTitle(""); // Pusty tytuł, bo mamy osobny widget z nagłówkiem
        chart->legend()->setVisible(false);

        // Tworzymy osie
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("yy.MM.dd HH:mm:ss");
        axisX->setTitleText("Data pomiaru");
        chart->setAxisX(axisX, scatterSeries); // Ustawiamy oś X dla scatterSeries
        chart->setAxisX(axisX, lineSeries);    // Ustawiamy oś X dla lineSeries

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("µg/m3 (" + paramFormula + ")");
        chart->setAxisY(axisY, scatterSeries); // Ustawiamy oś Y dla scatterSeries
        chart->setAxisY(axisY, lineSeries);    // Ustawiamy oś Y dla lineSeries

        // Dodaj marginesy do zakresu wartości osi Y
        if (hasData) {
            double yRange = maxValue - minValue;
            double yMargin = yRange * 0.1; // 10% marginesy
            if (yRange > 0) {
                axisY->setRange(minValue - yMargin, maxValue + yMargin);
            } else {
                // Domyślny zakres, jeśli wszystkie wartości są takie same
                axisY->setRange(minValue * 0.9, minValue * 1.1);
            }
        } else {
            // Domyślny zakres, jeśli nie ma danych
            axisY->setRange(0, 10);
        }

        // Tworzenie widoku wykresu
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setMinimumSize(600, 400);

        // --- Tworzenie widgetu dla statystyk ---
        QWidget *statsWidget = new QWidget();
        QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);

        // Nagłówek statystyk
        QLabel *statsHeader = new QLabel("Statystyki");
        statsHeader->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
        statsLayout->addWidget(statsHeader);

        // Etykiety dla statystyk
        QLabel *countLabel = new QLabel(QString("Liczba pomiarów: %1").arg(hasData ? QString::number(validCount) : "-"));

        // Użyj znaku "-", gdy nie ma pomiarów
        QString minValueText = hasData ? QString::number(minValue, 'f', 2) : "-";
        QString avgValueText = hasData ? QString::number(avgValue, 'f', 2) : "-";
        QString maxValueText = hasData ? QString::number(maxValue, 'f', 2) : "-";

        QLabel *minLabel = new QLabel(QString("Wartość minimalna: %1 %2").arg(minValueText).arg(hasData ? "ug/m3" : ""));
        QLabel *avgLabel = new QLabel(QString("Wartość średnia: %1 %2").arg(avgValueText).arg(hasData ? "ug/m3" : ""));
        QLabel *maxLabel = new QLabel(QString("Wartość maksymalna: %1 %2").arg(maxValueText).arg(hasData ? "ug/m3" : ""));

        // Dodaj informację, gdy nie ma danych
        if (!hasData) {
            QLabel *noDataLabel = new QLabel("Brak danych dla tego parametru");
            noDataLabel->setStyleSheet("color: red; font-size: 14px; margin-top: 10px;");
            statsLayout->addWidget(noDataLabel);
        }

        // Stylizacja etykiet
        QString labelStyle = "font-size: 14px; margin: 5px;";
        countLabel->setStyleSheet(labelStyle);
        minLabel->setStyleSheet(labelStyle);
        avgLabel->setStyleSheet(labelStyle);
        maxLabel->setStyleSheet(labelStyle);

        // Dodanie etykiet do layoutu statystyk
        statsLayout->addWidget(countLabel);
        statsLayout->addWidget(minLabel);
        statsLayout->addWidget(avgLabel);
        statsLayout->addWidget(maxLabel);

        // Dodaj spacer, aby wyrównać statystyki do góry
        statsLayout->addStretch();

        // Dodaj widok wykresu i statystyki do poziomego layoutu
        chartStatLayout->addWidget(chartView, 9); // proporcja 7
        chartStatLayout->addWidget(statsWidget, 1); // proporcja 3

        // Dodaj cały kontener do głównego layoutu
        mainLayout->addWidget(chartStatContainer);

        // 🔄 Sprawdzamy, czy okno wykresu już istnieje, jeśli tak, to je usuwamy i wyświetlamy nowe okno
        if (chartWindow) {
            chartWindow->close();
            delete chartWindow;
        }

        // Tworzymy nowe okno wykresu
        chartWindow = new QMainWindow();
        chartWindow->setWindowTitle(QString("%1 - %2").arg(stationName).arg(paramName));
        chartWindow->setCentralWidget(mainWidget);
        chartWindow->resize(1000, 550);  // Zwiększony rozmiar okna, aby zmieścić statystyki
        chartWindow->show();

        // Własny tooltip
        QLabel *customTooltip = new QLabel(nullptr);
        customTooltip->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        customTooltip->setAttribute(Qt::WA_ShowWithoutActivating);
        customTooltip->hide();

        QObject::connect(scatterSeries, &QScatterSeries::hovered, this,
                         [this, customTooltip, avgValue](const QPointF &point, bool state) {
                             if (!customTooltip) return;

                             if (state) {
                                 // Konwertowanie czasu z point.x() na QDateTime
                                 QDateTime date = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x()));

                                 // Pobieranie wartości dla y
                                 double value = point.y();

                                 // Obliczanie odchylenia od średniej
                                 double deviation = value - avgValue;
                                 QString deviationText = (deviation >= 0) ?
                                                             QString("+%1").arg(deviation, 0, 'f', 2) :
                                                             QString("%1").arg(deviation, 0, 'f', 2);

                                 // Tworzymy tekst tooltipa z dodatkową informacją o odchyleniu od średniej
                                 QString text = QString("Data: %1\nWartość: %2\nOdchylenie od średniej: %3")
                                                    .arg(date.toString("yyyy-MM-dd HH:mm:ss"))
                                                    .arg(value)
                                                    .arg(deviationText);

                                 // Ustawiamy tekst tooltipa i pokazujemy go
                                 customTooltip->setText(text);
                                 customTooltip->setStyleSheet(
                                     "background-color: white; "
                                     "color: black; "
                                     "border: 1px solid black; "
                                     "padding: 5px;"
                                     );
                                 customTooltip->adjustSize();
                                 customTooltip->move(QCursor::pos() + QPoint(10, 10));
                                 customTooltip->show();
                             } else {
                                 customTooltip->hide();
                             }
                         });

    },
                             [this](int errorCode) {
                                 if (error == false) {
                                     // Tworzymy nowy QLabel z komunikatem o błędzie
                                     errorLabel = new QLabel("Błąd połączenia");
                                     errorLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: red; padding: 20px;");
                                     parentLayout->addWidget(errorLabel);
                                     errorLabel->setAlignment(Qt::AlignCenter);
                                     error = true;
                                 }
                             });
}

