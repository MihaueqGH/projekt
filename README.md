Dane pogodowe

Projekt Dane pogodowe to aplikacja stworzona w technologii Qt, która pobiera dane pogodowe z polskich stacji meteorologicznych i generuje wykresy oraz przeprowadza prostą analizę tych danych. Aplikacja umożliwia również przeglądanie danych offline, po ich pobraniu do lokalnego folderu.
Funkcje

    Pobieranie danych: Aplikacja pobiera dane pogodowe ze stacji meteorologicznych w Polsce.

    Analiza danych: Po pobraniu danych, aplikacja generuje wykresy i przeprowadza prostą analizę, umożliwiając użytkownikowi szybkie zrozumienie bieżącej sytuacji pogodowej.

    Offline: Wszystkie dane są pobierane do lokalnego folderu, co umożliwia ich przeglądanie offline w późniejszym czasie.

    Interfejs użytkownika: Intuicyjny interfejs graficzny stworzony za pomocą Qt Widgets, umożliwiający łatwą nawigację i interakcję z danymi.

Wymagania

    Qt: Wersja 6.8.2 lub nowsza

    Kompilator: MinGW 13.1.0

    C++: Wersja C++17 lub nowsza

    Doxygen: Do generowania dokumentacji

    Środowisko: Windows 10 lub nowszy

Instalacja

    Instalacja zależności:

        Zainstaluj Qt 6.8.2.

        Zainstaluj MinGW 13.1.0.

        Zainstaluj Doxygen do generowania dokumentacji.

    Pobierz lub sklonuj repozytorium:

    git clone https://github.com/TwojeRepozytorium/dane-pogodowe.git

    Budowanie projektu:

        Otwórz projekt w Qt Creator.

        Skonfiguruj projekt za pomocą qmake.

        Zbuduj projekt.

    Uruchomienie:

        Uruchom aplikację, aby rozpocząć pobieranie danych z polskich stacji meteorologicznych.

        Po pobraniu danych, aplikacja wygeneruje wykresy i umożliwi ich przeglądanie offline.

Struktura projektu

/Dane_pogodowe
│
├── /api.cpp          # Kod odpowiedzialny za pobieranie danych z API
├── /mainwindow.cpp   # Główna logika aplikacji i interfejsu użytkownika
├── /showOffline.cpp  # Logika wyświetlania danych offline
├── /stationTable.cpp # Logika przetwarzania danych stacji
├── /timer.cpp        # Timer do odświeżania danych
│
├── /api.h            # Deklaracje funkcji do pobierania danych
├── /mainwindow.h     # Deklaracje klasy głównego okna aplikacji
├── /showOffline.h    # Deklaracje funkcji wyświetlania danych offline
├── /stationTable.h   # Deklaracje klasy do przetwarzania danych
├── /timer.h          # Deklaracje funkcji do obsługi timera
│
├── /mainwindow.ui    # Definicja interfejsu użytkownika
└── /icon.qrc         # Ikony wykorzystywane w aplikacji