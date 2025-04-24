#include "showOffline.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonArray>
#include <QDir>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHeaderView>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QScatterSeries>
#include <QValueAxis>
#include <QVector>
#include <QToolTip>
#include <QComboBox>
#include <QDebug>
/**
     * @brief Konstruktor klasy ShowOffline.
     * @param parentLayout Layout, do którego zostaną dodane elementy interfejsu użytkownika.
     */
ShowOffline::ShowOffline(QVBoxLayout *parentLayout)
    : QWidget(), parentLayout(parentLayout)
{
    // Tworzymy pole tekstowe do filtrowania
    filterEdit = new QLineEdit();
    filterEdit->setPlaceholderText("Filtruj po stacjach i sensorach");
    filterEdit->setFixedHeight(30);

    // Tworzymy tabelę
    tableWidget = new QTableWidget(0, 1);  // Jedna kolumna
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Stacja | Sensor");

    // Ustawiamy, by tabela nie była edytowalna
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Sprawiamy, że kolumna rozciąga się na całą szerokość
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setStyleSheet(R"(
    QTableWidget::item:hover {
        background-color: rgba(255,255,255,100);
    }
)");

    // Dodajemy pole tekstowe i tabelę do layoutu
    parentLayout->addWidget(filterEdit);
    parentLayout->addWidget(tableWidget);

    // Podłączamy sygnał zmiany tekstu do funkcji filtrowania
    QObject::connect(filterEdit, &QLineEdit::textChanged, [this](const QString &text) {
        filterTable(text);
    });

    // Wyświetlamy dane z plików
    showDataFromFiles();

    // Podłączamy sygnał kliknięcia w wiersz do obsługi
    QObject::connect(tableWidget, &QTableWidget::cellClicked, this, &ShowOffline::onRowClicked);
}

ShowOffline::~ShowOffline()
{
    delete tableWidget;
    delete filterEdit;
}
/**
 * @brief Wczytuje dane z plików JSON znajdujących się w katalogu "data/".
 *
 * Jeśli folder nie istnieje lub nie zawiera plików, wyświetlany jest odpowiedni komunikat.
 * Dane z plików są przetwarzane i dodawane jako wiersze do tabeli.
 */

void ShowOffline::showDataFromFiles()
{
    QDir dir(QDir::currentPath() + "/data");

    // Sprawdzamy, czy folder istnieje
    if (!dir.exists()) {
        if (!errorLabel) {
            // Tworzymy etykietę, jeśli jeszcze nie została utworzona
            errorLabel = new QLabel("Nie znaleziono zapisanych danych", this);
            errorLabel->setStyleSheet("color: red;font-size:24px");
            parentLayout->addWidget(errorLabel);
        }
        return;
    } else {
        if (errorLabel) {
            errorLabel->clear();  // Czyszczenie błędu, jeśli folder istnieje
        }
    }

    // Pobieramy wszystkie pliki JSON w folderze
    QStringList files = dir.entryList(QStringList() << "*.json", QDir::Files);

    if (files.isEmpty()) {
        if (!errorLabel) {
            // Tworzymy etykietę, jeśli nie ma żadnych plików JSON
            errorLabel = new QLabel("Nie znaleziono zapisanych danych", this);
            errorLabel->setStyleSheet("color: red;font-size:24px");
            parentLayout->addWidget(errorLabel);
        }
        return;
    } else {
        if (errorLabel) {
            errorLabel->clear();  // Czyszczenie błędu, jeśli pliki JSON są obecne
        }
    }

    // Kod do przetwarzania plików JSON
    for (const QString &fileName : files) {
        QFile file(dir.absoluteFilePath(fileName));
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray fileData = file.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(fileData);
            QJsonObject dataObj = doc.object();

            // Przetwarzamy dane z JSON
            parseJsonData(dataObj, fileName);

            file.close();
        }
    }
}

/**
 * @brief Przetwarza pojedynczy obiekt JSON i dodaje go do tabeli.
 * @param dataObj Obiekt JSON zawierający dane o stacji i sensorze.
 * @param fileName Nazwa pliku źródłowego, używana do identyfikacji sensora.
 */

void ShowOffline::parseJsonData(const QJsonObject &dataObj, const QString &fileName)
{
    QString stationName = dataObj["stationName"].toString();
    QString paramName = dataObj["paramName"].toString();
    QString paramFormula = dataObj["key"].toString();
    QString sensorId = fileName.section('_', 1, 1).section('.', 0, 0);  // Wyciągamy sensorId z nazwy pliku

    // Scalony tekst z informacjami o stacji i sensorze
    QString combinedText = QString("Stacja: %1  |  Sensor: %2 (%3)")
                               .arg(stationName, paramName, paramFormula);

    // Dodajemy wiersz do tabeli
    addRowToTable(combinedText, sensorId);
}
/**
 * @brief Dodaje nowy wiersz do tabeli z opisem stacji i sensora.
 * @param combinedText Tekst wyświetlany w tabeli.
 * @param sensorId Identyfikator sensora, przechowywany jako dane użytkownika.
 */

void ShowOffline::addRowToTable(const QString &combinedText, const QString &sensorId)
{
    int rowCount = tableWidget->rowCount();
    tableWidget->insertRow(rowCount);

    // Dodajemy scalony tekst do tabeli w jednej kolumnie
    QTableWidgetItem *item = new QTableWidgetItem(combinedText);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);  // Blokujemy edycję

    // Przechowujemy sensorId w danych wiersza
    item->setData(Qt::UserRole, sensorId);

    tableWidget->setItem(rowCount, 0, item);
    tableWidget->setRowHeight(rowCount,35);
}
/**
 * @brief Filtrowanie tabeli na podstawie wpisanego tekstu.
 * @param filterText Tekst filtrowania; pasujące wiersze są widoczne, pozostałe ukryte.
 */

void ShowOffline::filterTable(const QString &filterText)
{
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (item && item->text().contains(filterText, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        tableWidget->setRowHidden(row, !match);  // Ukrywamy wiersz, jeśli nie pasuje
    }
}
/**
 * @brief Obsługuje kliknięcie w wiersz tabeli i wyświetla wykres dla wybranego sensora.
 * @param row Indeks klikniętego wiersza.
 * @param column Indeks klikniętej kolumny (zawsze 0 w tym przypadku).
 */

void ShowOffline::onRowClicked(int row, int column)
{
    // Pobieramy sensorId z wybranego wiersza
    QTableWidgetItem *item = tableWidget->item(row, column);
    QString sensorId = item->data(Qt::UserRole).toString();

    // Otwieramy plik JSON odpowiadający sensorId
    QString fileName = QDir::currentPath() + "/data/data_" + sensorId + ".json";
    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        QJsonObject dataObj = doc.object();

        // Tworzymy wykres na podstawie danych z pliku
        createChart(dataObj);

        file.close();
    } else {
        QMessageBox::warning(this, "Błąd", "Nie znaleziono pliku danych.");
    }
}



/**
 * @brief Tworzy i wyświetla wykres z danymi sensora oraz podstawowymi statystykami.
 * @param dataObj Obiekt JSON zawierający dane pomiarowe.
 *
 * Funkcja obsługuje także wybór zakresu dat, filtruje dane oraz aktualizuje widok i statystyki.
 */

void ShowOffline::createChart(const QJsonObject &dataObj)
{
    // --- Zarządzanie oknem i tooltipem ---
    // Zamknij i usuń poprzednie okno wykresu, jeśli istnieje
    if (chartWindow) {
        chartWindow->close();
        // Użyj deleteLater, aby bezpiecznie usunąć obiekt po zakończeniu bieżącego zdarzenia
        chartWindow->deleteLater();
        chartWindow = nullptr;
    }
    // Usuń poprzedni tooltip, jeśli istnieje
    if (customTooltip) {
        customTooltip->deleteLater();
        customTooltip = nullptr;
    }

    // Zapisz aktualne dane na potrzeby ponownego rysowania po zmianie zakresu dat
    m_currentDataObj = dataObj;

    // --- Przygotowanie danych podstawowych ---
    QString stationName = dataObj["stationName"].toString();
    QString paramName = dataObj["paramName"].toString();
    QString paramFormula = dataObj["key"].toString();
    QJsonArray allValues = dataObj["values"].toArray(); // Pobierz wszystkie wartości

    // --- Okno i layout ---
    chartWindow = new QWidget();
    chartWindow->setWindowTitle(QString("%0 - %1").arg(stationName, paramName));
    QVBoxLayout *mainLayout = new QVBoxLayout(chartWindow); // Główny layout okna

    // --- ComboBox do wyboru zakresu dat ---
    if (!m_dateRangeComboBox) {
        m_dateRangeComboBox = new QComboBox(chartWindow);
        m_dateRangeComboBox->addItems({"Wszystkie dane", "Ostatni miesiąc", "Ostatnie 3 dni"});

        connect(m_dateRangeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this](int index) {
                    Q_UNUSED(index);
                    if (!m_currentDataObj.isEmpty()) {
                        createChart(m_currentDataObj);
                    }
                });
    }
    // Dodaj ComboBox do layoutu (na górze)
    mainLayout->addWidget(m_dateRangeComboBox);

    // --- Filtrowanie danych na podstawie wyboru w ComboBox ---
    int selectedRangeIndex = m_dateRangeComboBox->currentIndex();
    QJsonArray filteredValues; // Tablica na przefiltrowane wartości
    QDateTime now = QDateTime::currentDateTime();
    QDateTime cutoffDate;

    if (selectedRangeIndex == 1) { // Ostatni miesiąc
        cutoffDate = now.addMonths(-1);
    } else if (selectedRangeIndex == 2) { // Ostatnie 3 dni
        cutoffDate = now.addDays(-3);
    }

    // Zmienne do obliczeń statystycznych
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    double sumValues = 0.0;
    int countValues = 0;

    for (const QJsonValue &value : allValues) {
        QJsonObject valueObj = value.toObject();
        if (valueObj.contains("value") && valueObj["value"].isNull()) {
            continue; // Pomiń wartości null
        }

        QDateTime currentDateTime = QDateTime::fromString(valueObj["date"].toString(), "yyyy-MM-dd HH:mm:ss");
        double valueY = valueObj["value"].toDouble();

        bool includePoint = true;
        if (selectedRangeIndex == 1 || selectedRangeIndex == 2) {
            if (currentDateTime < cutoffDate) {
                includePoint = false;
            }
        }

        if (includePoint) {
            filteredValues.append(value); // Dodaj punkt do przefiltrowanej tablicy

            // Aktualizuj statystyki
            minValue = qMin(minValue, valueY);
            maxValue = qMax(maxValue, valueY);
            sumValues += valueY;
            countValues++;
        }
    }

    // Oblicz średnią tylko jeśli są jakieś pomiary
    double avgValue = (countValues > 0) ? (sumValues / countValues) : 0.0;

    // Sprawdź, czy mamy jakiekolwiek pomiary
    bool hasData = (countValues > 0);

    // Utwórz widget z layoutem poziomym dla wykresu i statystyk
    QWidget *chartStatContainer = new QWidget();
    QHBoxLayout *chartStatLayout = new QHBoxLayout(chartStatContainer);

    // --- Tworzenie wykresu (Chart) ---
    QChart *chart = new QChart();
    chart->legend()->setVisible(false);
    chart->setTitle(QString("%0 - %1").arg(stationName, paramName));

    // Dodaj marginesy do wykresu, aby linie nie dotykały krawędzi
    chart->setMargins(QMargins(20, 20, 20, 20));

    // --- Tworzenie serii danych (Series) ---
    QScatterSeries *scatterSeries = new QScatterSeries();
    scatterSeries->setMarkerSize(8);

    QVector<QLineSeries*> lineSeriesVector;
    QLineSeries *currentLineSeries = new QLineSeries();
    lineSeriesVector.append(currentLineSeries);

    QDateTime prevDateTime;
    bool isFirstPoint = true;

    // Używamy przefiltrowanych danych 'filteredValues'
    for (const QJsonValue &value : filteredValues) {
        QJsonObject valueObj = value.toObject();

        if (valueObj.contains("value") && valueObj["value"].isNull()) {
            continue;
        }

        QDateTime currentDateTime = QDateTime::fromString(valueObj["date"].toString(), "yyyy-MM-dd HH:mm:ss");
        double valueY = valueObj["value"].toDouble();

        // Dodaj punkt do serii rozproszonej
        scatterSeries->append(currentDateTime.toMSecsSinceEpoch(), valueY);

        // Logika łamania linii dla serii liniowej
        if (!isFirstPoint) {
            int secondsDiff = prevDateTime.secsTo(currentDateTime);
            // Jeśli różnica czasu jest większa niż 1 godzina (3600 sekund), zacznij nową linię
            if (secondsDiff > 3600) {
                if (currentLineSeries->count() > 0) {
                    // Seria zostanie dodana do wykresu później, w pętli poniżej
                } else {
                    lineSeriesVector.removeLast();
                    delete currentLineSeries;
                }
                currentLineSeries = new QLineSeries();
                lineSeriesVector.append(currentLineSeries);
            }
        }

        // Dodaj punkt do bieżącej serii liniowej
        currentLineSeries->append(currentDateTime.toMSecsSinceEpoch(), valueY);
        prevDateTime = currentDateTime;
        isFirstPoint = false;
    }

    // --- Dodawanie serii do wykresu ---
    chart->addSeries(scatterSeries); // Dodaj serię rozproszoną

    // Dodaj wszystkie serie liniowe (segmenty) do wykresu
    for (QLineSeries *series : lineSeriesVector) {
        // Dodawaj tylko serie, które mają co najmniej 2 punkty (aby tworzyły linię)
        if (series->count() > 1) {
            chart->addSeries(series);
        } else {
            delete series; // Zwolnij pamięć
        }
    }

    // --- Konfiguracja osi ---
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yy.MM.dd HH:mm:ss"); // Format daty na osi X
    axisX->setTitleText("Data pomiaru");

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(QString("ug/m3 (%0)").arg(paramFormula));

    // Dodaj dodatkowe marginesy do zakresu wartości osi Y, aby linie nie dotykały krawędzi
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

    // --- Przypisanie osi do serii ---
    chart->setAxisX(axisX, scatterSeries);
    chart->setAxisY(axisY, scatterSeries);

    // Przypisz te same osie do wszystkich serii liniowych
    for (QAbstractSeries *series : chart->series()) {
        // Pomijamy już przypisaną serię rozproszoną
        if (series != scatterSeries) {
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
        }
    }

    // --- Widok wykresu (Chart View) ---
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(600, 400); // Zmniejszony rozmiar minimalny, aby zmieścić statystyki

    // --- Tworzenie widgetu dla statystyk ---
    QWidget *statsWidget = new QWidget();
    QVBoxLayout *statsLayout = new QVBoxLayout(statsWidget);

    // Nagłówek statystyk
    QLabel *statsHeader = new QLabel("Statystyki");
    statsHeader->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    statsLayout->addWidget(statsHeader);

    // Etykiety dla statystyk
    QString rangeText;
    switch (selectedRangeIndex) {
    case 0: rangeText = "Wszystkie dane"; break;
    case 1: rangeText = "Ostatni miesiąc"; break;
    case 2: rangeText = "Ostatnie 3 dni"; break;
    }

    QLabel *rangeLabel = new QLabel(QString("Zakres: %1").arg(rangeText));
    QLabel *countLabel = new QLabel(QString("Liczba pomiarów: %1").arg(hasData ? QString::number(countValues) : "-"));

    // Użyj znaku "-", gdy nie ma pomiarów
    QString minValueText = hasData ? QString::number(minValue, 'f', 2) : "-";
    QString avgValueText = hasData ? QString::number(avgValue, 'f', 2) : "-";
    QString maxValueText = hasData ? QString::number(maxValue, 'f', 2) : "-";

    QLabel *minLabel = new QLabel(QString("Wartość minimalna: %1 %2").arg(minValueText).arg(hasData ? "ug/m3" : ""));
    QLabel *avgLabel = new QLabel(QString("Wartość średnia: %1 %2").arg(avgValueText).arg(hasData ? "ug/m3" : ""));
    QLabel *maxLabel = new QLabel(QString("Wartość maksymalna: %1 %2").arg(maxValueText).arg(hasData ? "ug/m3" : ""));

    // Dodaj informację, gdy nie ma danych w wybranym zakresie
    if (!hasData) {
        QLabel *noDataLabel = new QLabel("Brak danych w wybranym zakresie");
        noDataLabel->setStyleSheet("color: red; font-size: 14px; margin-top: 10px;");
        statsLayout->addWidget(noDataLabel);
    }

    // Stylizacja etykiet
    QString labelStyle = "font-size: 14px; margin: 5px;";
    rangeLabel->setStyleSheet(labelStyle);
    countLabel->setStyleSheet(labelStyle);
    minLabel->setStyleSheet(labelStyle);
    avgLabel->setStyleSheet(labelStyle);
    maxLabel->setStyleSheet(labelStyle);

    // Dodanie etykiet do layoutu statystyk
    statsLayout->addWidget(rangeLabel);
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

    // --- Konfiguracja okna ---
    chartWindow->resize(1000, 550); // Zwiększony rozmiar okna, aby zmieścić statystyki
    chartWindow->show();

    // --- Konfiguracja niestandardowego Tooltipa ---
    customTooltip = new QLabel(nullptr);
    customTooltip->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    customTooltip->setAttribute(Qt::WA_ShowWithoutActivating);
    customTooltip->hide();

    // Połącz sygnał najechania myszką na punkt w serii rozproszonej
    connect(scatterSeries, &QScatterSeries::hovered, this,
            [this, avgValue](const QPointF &point, bool state) {
                if (!customTooltip) return;

                if (state) {
                    // Obliczamy odchylenie od średniej
                    double deviation = point.y() - avgValue;
                    QString deviationStr = QString::number(deviation, 'f', 2);

                    // Dodajemy znak + dla dodatnich odchyleń dla lepszej czytelności
                    if (deviation > 0) {
                        deviationStr = "+" + deviationStr;
                    }

                    // Formatowanie tekstu tooltipa z nową informacją o odchyleniu
                    QString text = QString("Data: %1\nWartość: %2\nOdchylenie od średniej: %3")
                                       .arg(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x())).toString("yyyy-MM-dd HH:mm:ss"))
                                       .arg(point.y())
                                       .arg(deviationStr);

                    customTooltip->setText(text);

                    customTooltip->setStyleSheet(
                        "background-color: white; "
                        "color: black; "
                        "border: 1px solid black; "
                        "padding: 5px;"
                        );
                    customTooltip->adjustSize();

                    // Uzyskaj pozycję kursora globalną
                    QPoint globalCursorPos = QCursor::pos();
                    customTooltip->move(globalCursorPos + QPoint(15, 10));

                    customTooltip->show();
                } else {
                    customTooltip->hide();
                }
            });
}
