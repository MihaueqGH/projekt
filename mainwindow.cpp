#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include "api.h"
#include <QChartView>
#include "showOffline.h"

/**
 * @brief Konstruktor klasy MainWindow.
 * Inicjalizuje interfejs, ustawia rozmiar okna, ikonę, tytuł oraz uruchamia ekran informacyjny "O aplikacji".
 * @param parent Wskaźnik do obiektu nadrzędnego.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setFixedSize(1300, 800); // Ustala stały rozmiar okna
    timeDisplay = new Timer(ui->timeLabel, this);
    setWindowIcon(QIcon(":/icons/icon.png"));
    this->setWindowTitle("Dane pogodowe - Projekt");
    on_about_clicked(); // Pokazuje informacje o aplikacji na start
}

/**
 * @brief Destruktor klasy MainWindow.
 * Czyści layout, usuwa wskaźniki i zwalnia zasoby.
 */
MainWindow::~MainWindow()
{
    MainWindow::clearLayout(ui->mainLayout, ui->mainInformation);
    delete stationTable;
    delete timeDisplay;
    delete manager;
}

/**
 * @brief Slot obsługujący przycisk "Pokaż wszystkie dane".
 * Czyści główny layout, tworzy nową tabelę stacji i pobiera dane z API.
 */
void MainWindow::on_showAll_clicked()
{
    MainWindow::clearLayout(ui->mainLayout, ui->mainInformation);
    stationTable = new StationTable(ui->mainLayout);
    ui->mainInformation->setText("Wszystkie stacje pogodowe");

    ApiManager::fetchData("https://api.gios.gov.pl/pjp-api/rest/station/findAll",
                          [this](const QJsonArray &data) {
                              stationTable->showData(data); // Wyświetlenie danych po otrzymaniu
                          },
                          [this](int errorCode) {
                              // Obsługa błędu połączenia
                              for (int i = ui->mainLayout->count() - 1; i >= 0; --i) {
                                  QWidget *widget = ui->mainLayout->itemAt(i)->widget();
                                  if (widget) {
                                      delete widget;
                                  }
                              }

                              QLabel *errorLabel = new QLabel("Błąd połączenia");
                              errorLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: red; padding: 20px;");
                              ui->mainLayout->addWidget(errorLabel);
                              errorLabel->setAlignment(Qt::AlignCenter);
                          });
}

/**
 * @brief Czyści wskazany layout z widgetów i przywraca tekst etykiety.
 *
 * @param layout Layout do wyczyszczenia.
 * @param mainInformation QLabel, który ma pozostać nienaruszony i którego tekst zostanie zaktualizowany.
 */
void MainWindow::clearLayout(QLayout *layout, QLabel *mainInformation) {
    if (!layout) return;

    // Usuwanie wykresów, przycisków i etykiet
    for (int i = layout->parentWidget()->layout()->count() - 1; i >= 0; --i) {
        QWidget *widget = layout->parentWidget()->layout()->itemAt(i)->widget();
        if (widget && qobject_cast<QChartView*>(widget)) delete widget;
        if (widget && qobject_cast<QPushButton*>(widget)) delete widget;
        if (widget && qobject_cast<QLabel*>(widget)) delete widget;
    }

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->hide();
            widget->deleteLater(); // Opóźnione usunięcie
        } else if (QLayout *childLayout = item->layout()) {
            clearLayout(childLayout, mainInformation); // Rekurencyjne czyszczenie
        }
        delete item;
    }

    mainInformation->setText("Dane pogodowe - Projekt");
}

/**
 * @brief Slot obsługujący tryb offline.
 * Tworzy widok danych z plików lokalnych.
 */
void MainWindow::on_showOffline_clicked()
{
    MainWindow::clearLayout(ui->mainLayout, ui->mainInformation);
    ui->mainInformation->setText("Dane offline");
    ShowOffline *showOffline = new ShowOffline(ui->mainLayout);
}

/**
 * @brief Slot wyświetlający informacje "O aplikacji".
 * Tworzy opis funkcji aplikacji oraz link do API GIOŚ.
 */
void MainWindow::on_about_clicked()
{
    MainWindow::clearLayout(ui->mainLayout, ui->mainInformation);
    ui->mainInformation->setText("O aplikacji");

    QLabel *aboutInfo = new QLabel();
    aboutInfo->setWordWrap(true);
    aboutInfo->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    aboutInfo->setStyleSheet("font-size: 14px; padding: 20px;");

    aboutInfo->setText(
        "<h2>Aplikacja Dane Pogodowe</h2>"
        "<p>Aplikacja służy do wyświetlania danych pogodowych ze stacji pomiarowych rozmieszczonych na terenie Polski.</p>"
        "<h3>Funkcje aplikacji:</h3>"
        "<ul>"
        "<li>Pobieranie aktualnych danych ze stacji pogodowych</li>"
        "<li>Lokalne zapisywanie pobranych danych na urządzeniu</li>"
        "<li>Tworzenie wykresów na podstawie zebranych danych</li>"
        "<li>Analiza statystyczna danych</li>"
        "<li>Dostęp do danych offline</li>"
        "</ul>"
        "<p>Projekt wykorzystuje API do pobierania aktualnych danych pomiarowych - "
        "<a href='https://powietrze.gios.gov.pl/pjp/content/api'>https://powietrze.gios.gov.pl/pjp/content/api</a></p>"
        );

    aboutInfo->setOpenExternalLinks(true);
    ui->mainLayout->addWidget(aboutInfo);
}
