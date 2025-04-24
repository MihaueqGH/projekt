#ifndef SHOWOFFLINE_H
#define SHOWOFFLINE_H

#include <QWidget>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QComboBox>

/**
 * @class ShowOffline
 * @brief Klasa odpowiedzialna za wyświetlanie danych pomiarowych zapisanych lokalnie (offline).
 *
 * Klasa odczytuje pliki JSON z katalogu "data/", filtruje dane,
 * umożliwia przeglądanie listy stacji i sensorów oraz generuje wykresy
 * z podstawowymi statystykami dla wybranego sensora.
 */
class ShowOffline : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy ShowOffline.
     * @param parentLayout Layout, do którego zostaną dodane elementy interfejsu użytkownika.
     */
    explicit ShowOffline(QVBoxLayout *parentLayout);

    /**
     * @brief Destruktor klasy ShowOffline.
     */
    ~ShowOffline();

private:
    QLineEdit *filterEdit;               /**< Pole edycyjne do filtrowania wpisów w tabeli */
    QTableWidget *tableWidget;           /**< Tabela wyświetlająca listę stacji i sensorów */
    QVBoxLayout *parentLayout;           /**< Layout nadrzędny, do którego dodawane są elementy GUI */
    QWidget *chartWindow = nullptr;      /**< Okno wykresu wyświetlające dane sensora */
    QLabel *customTooltip = nullptr;     /**< Etykieta używana jako własny tooltip */
    QJsonObject m_currentDataObj;        /**< Obiekt JSON przechowujący dane do ponownego rysowania wykresu */
    QComboBox *m_dateRangeComboBox = nullptr; /**< Pole wyboru zakresu dat dla wykresu */
    QLabel *errorLabel = nullptr;        /**< Etykieta do wyświetlania komunikatów o błędach */

    /**
     * @brief Wczytuje dane z plików JSON znajdujących się w katalogu "data/".
     */
    void showDataFromFiles();

    /**
     * @brief Przetwarza pojedynczy obiekt JSON i dodaje go do tabeli.
     * @param dataObj Obiekt JSON zawierający dane o stacji i sensorze.
     * @param fileName Nazwa pliku źródłowego, używana do identyfikacji sensora.
     */
    void parseJsonData(const QJsonObject &dataObj, const QString &fileName);

    /**
     * @brief Dodaje nowy wiersz do tabeli z opisem stacji i sensora.
     * @param combinedText Tekst wyświetlany w tabeli.
     * @param sensorId Identyfikator sensora, przechowywany jako dane użytkownika.
     */
    void addRowToTable(const QString &combinedText, const QString &sensorId);

    /**
     * @brief Filtrowanie tabeli na podstawie wpisanego tekstu.
     * @param filterText Tekst filtrowania; pasujące wiersze są widoczne, pozostałe ukryte.
     */
    void filterTable(const QString &filterText);

    /**
     * @brief Obsługuje kliknięcie w wiersz tabeli i wyświetla wykres dla wybranego sensora.
     * @param row Indeks klikniętego wiersza.
     * @param column Indeks klikniętej kolumny.
     */
    void onRowClicked(int row, int column);

    /**
     * @brief Tworzy i wyświetla wykres z danymi sensora oraz podstawowymi statystykami.
     * @param dataObj Obiekt JSON zawierający dane pomiarowe.
     */
    void createChart(const QJsonObject &dataObj);
};

#endif // SHOWOFFLINE_H
