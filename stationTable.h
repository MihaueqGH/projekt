#ifndef STATIONTABLE_H
#define STATIONTABLE_H

#include <QTableWidget>
#include <QJsonArray>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QJsonObject>
#include <QMainWindow>
#include <QChartView>
#include <QComboBox>
#include <QDateEdit>
#include <QJsonDocument>

/**
 * @brief Klasa StationTable służy do wyświetlania tabeli ze stacjami pomiarowymi,
 *        umożliwia filtrowanie, wybór stacji oraz prezentację danych w formie wykresu.
 */
class StationTable : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy StationTable.
     * @param parentLayout Layout, do którego dodawane będą komponenty GUI.
     */
    StationTable(QVBoxLayout *parentLayout);

    /**
     * @brief Destruktor klasy StationTable.
     */
    ~StationTable();

    /**
     * @brief Wyświetla dane stacji w tabeli.
     * @param data Tablica JSON z danymi stacji.
     */
    void showData(const QJsonArray &data);

    /**
     * @brief Zwraca wskaźnik do widżetu tabeli.
     * @return Wskaźnik do QTableWidget.
     */
    QTableWidget* getTableWidget();

private:
    /**
     * @brief Tworzy nowe okno z wykresem.
     * @param chartView Widok wykresu do wyświetlenia.
     * @param title Tytuł okna z wykresem.
     */
    void createChartWindow(QChartView *chartView, const QString &title);

    QTableWidget *table;               ///< Widżet tabeli z danymi stacji.
    QLineEdit *filterEdit;            ///< Pole do filtrowania zawartości tabeli.
    QMainWindow *chartWindow = nullptr; ///< Okno wykresu (jeśli zostało utworzone).
    QVBoxLayout *parentLayout;        ///< Referencja do layoutu głównego.
    QJsonArray data;                  ///< Dane stacji pobrane z API lub pliku.
    QLabel *errorLabel;               ///< Etykieta błędu do wyświetlania komunikatów.
    bool error = false;               ///< Flaga błędu dla operacji.

    /**
     * @brief Filtrowanie tabeli według tekstu.
     * @param filterText Tekst do filtrowania.
     */
    void filterTable(const QString &filterText);

    /**
     * @brief Obsługuje kliknięcie w komórkę tabeli.
     * @param row Indeks klikniętego wiersza.
     */
    void onCellClicked(int row);

    /**
     * @brief Wysyła zapytanie do API w celu pobrania danych stacji.
     * @param stationId ID wybranej stacji.
     * @param stationData Obiekt JSON z informacjami o stacji.
     */
    void fetchApi(int stationId, const QJsonObject &stationData);

    /**
     * @brief Pobiera dane z czujnika dla konkretnego parametru.
     * @param sensorId ID sensora.
     * @param stationData Informacje o stacji.
     * @param paramName Nazwa parametru (np. PM10).
     * @param paramFormula Wzór parametru (np. C6H6).
     */
    void fetchSensorData(int sensorId, const QJsonObject &stationData, const QString &paramName, const QString &paramFormula);

    /**
     * @brief Wyświetla dane (wartości) na wykresie po przefiltrowaniu.
     * @param filteredValues Przefiltrowane dane pomiarowe.
     */
    void showFilteredDataOnChart(const QJsonArray &filteredValues);

    /**
     * @brief Sortuje dane według daty (rosnąco).
     * @param valuesArray Tablica wartości do posortowania.
     */
    void sortDataByDate(QJsonArray &valuesArray);
};

#endif // STATIONTABLE_H
