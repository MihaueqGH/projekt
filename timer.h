#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <QLabel>
#include <QDateTime>

/**
 * @brief Klasa Timer umożliwia wyświetlanie aktualnego czasu na etykiecie.
 *
 * Ta klasa korzysta z obiektu QTimer do okresowego aktualizowania wyświetlanego czasu
 * na etykiecie QLabel. Czas jest aktualizowany w sposób automatyczny co sekundę.
 */
class Timer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy Timer.
     *
     * Inicjalizuje obiekt klasy Timer, tworzy i ustawia QTimer oraz przypisuje QLabel
     * do wyświetlania aktualnego czasu.
     *
     * @param label Wskaźnik na etykietę, na której będzie wyświetlany czas.
     * @param parent Wskaźnik na rodzica (domyślnie nullptr).
     */
    explicit Timer(QLabel *label, QObject *parent = nullptr);

    /**
     * @brief Destruktor klasy Timer.
     *
     * Usuwa obiekt QTimer.
     */
    ~Timer();

private slots:
    /**
     * @brief Slot do aktualizacji czasu na etykiecie.
     *
     * Ta metoda jest wywoływana przez QTimer co sekundę i aktualizuje wyświetlany
     * czas na etykiecie QLabel.
     */
    void updateTime();

private:
    QTimer *timer; /**< Wskaźnik na obiekt QTimer, który steruje aktualizacjami czasu. */
    QLabel *label; /**< Wskaźnik na etykietę QLabel, na której wyświetlany jest czas. */
};

#endif // TIMER_H
