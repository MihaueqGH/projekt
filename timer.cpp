#include "timer.h"

/**
 * @brief Konstruktor klasy Timer.
 *
 * Tworzy obiekt QTimer i łączy go z odpowiednim slotem do aktualizacji czasu.
 * Następnie wywołuje metodę updateTime() po raz pierwszy, aby natychmiast wyświetlić czas,
 * a potem uruchamia QTimer do aktualizacji czasu co sekundę.
 *
 * @param label Wskaźnik na etykietę QLabel, na której będzie wyświetlany czas.
 * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr).
 */
Timer::Timer(QLabel *label, QObject *parent)
    : QObject(parent), label(label)
{
    timer = new QTimer(this);  /**< Tworzymy nowy obiekt QTimer. */

    // Łączenie sygnału "timeout" z slotem "updateTime" w celu aktualizacji czasu
    connect(timer, &QTimer::timeout, this, &Timer::updateTime);

    updateTime(); // Wywołanie metody updateTime() raz na start, aby natychmiast wyświetlić czas

    timer->start(1000); // Rozpoczynamy odliczanie co 1000 ms (1 sekunda)
}

/**
 * @brief Destruktor klasy Timer.
 *
 * Zatrzymuje odliczanie QTimer po zakończeniu pracy obiektu.
 */
Timer::~Timer()
{
    timer->stop();  /**< Zatrzymanie QTimer, aby zapobiec dalszym aktualizacjom czasu po zniszczeniu obiektu. */
}

/**
 * @brief Slot do aktualizacji wyświetlanego czasu.
 *
 * Ta metoda jest wywoływana przez QTimer co sekundę. Pobiera bieżący czas systemowy
 * i aktualizuje tekst etykiety, aby wyświetlał ten czas w formacie "dd.MM.yyyy hh:mm:ss".
 */
void Timer::updateTime()
{
    QDateTime date = QDateTime::currentDateTime();  /**< Pobieramy bieżący czas systemowy. */

    QString dateText = date.toString("dd.MM.yyyy hh:mm:ss");  /**< Konwertujemy czas na łańcuch znaków w określonym formacie. */

    label->setText(dateText);  /**< Ustawiamy nowy tekst na etykiecie. */
}
