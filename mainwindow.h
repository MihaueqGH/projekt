#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QApplication>
#include "stationtable.h"
#include "timer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Główne okno aplikacji "Dane pogodowe".
 *
 * Klasa odpowiedzialna za interfejs użytkownika oraz zarządzanie
 * głównymi komponentami aplikacji, takimi jak tabela stacji,
 * zegar, dane offline i informacje o aplikacji.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     *
     * @param parent Wskaźnik do obiektu nadrzędnego (domyślnie nullptr).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot wywoływany po kliknięciu przycisku "Pokaż wszystkie dane".
     * @slot
     */
    void on_showAll_clicked();

    /**
     * @brief Slot wywoływany po kliknięciu przycisku "Tryb offline".
     * @slot
     */
    void on_showOffline_clicked();

    /**
     * @brief Slot wywoływany po kliknięciu przycisku "O programie".
     * @slot
     */
    void on_about_clicked();

private:
    Ui::MainWindow *ui; ///< Interfejs użytkownika wygenerowany przez Qt Designer.
    QTimer *timer; ///< Timer do aktualizacji czasu.
    QNetworkAccessManager *manager; ///< Obiekt do obsługi zapytań sieciowych.

    /**
     * @brief Czyści wszystkie widgety z danego layoutu.
     *
     * @param layout Layout, który ma zostać wyczyszczony.
     * @param mainInformation Etykieta z główną informacją, która nie zostanie usunięta.
     */
    void clearLayout(QLayout *layout, QLabel *mainInformation);

    Timer *timeDisplay; ///< Obiekt wyświetlający aktualny czas.
    StationTable *stationTable; ///< Obiekt odpowiadający za wyświetlanie i obsługę tabeli stacji.
};

#endif // MAINWINDOW_H
