#ifndef API_H
#define API_H

#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <functional>

/**
 * @brief Klasa ApiManager odpowiada za pobieranie danych z API.
 *
 * Klasa udostępnia metody statyczne do pobierania danych JSON
 * z podanego URL-a, z obsługą błędów za pomocą callbacków.
 */
class ApiManager {
public:
    /**
     * @brief Pobiera dane z API jako QJsonArray.
     *
     * @param urlString URL żądania.
     * @param callback Funkcja wywoływana po udanym pobraniu danych (z QJsonArray).
     * @param errorCallback Funkcja wywoływana w przypadku błędu (z kodem błędu).
     */
    static void fetchData(const QString &urlString,
                          std::function<void(const QJsonArray&)> callback,
                          std::function<void(int)> errorCallback);

    /**
     * @brief Pobiera dane z API jako surowy QJsonDocument.
     *
     * @param url URL żądania.
     * @param callback Funkcja wywoływana po udanym pobraniu danych (z QJsonDocument).
     * @param errorCallback Funkcja wywoływana w przypadku błędu (z kodem błędu).
     */
    static void fetchDataRaw(const QString &url,
                             std::function<void(const QJsonDocument &)> callback,
                             std::function<void(int)> errorCallback);

private:
    /**
     * @brief Parsuje dane JSON do QJsonArray.
     *
     * @param data Surowe dane JSON.
     * @return QJsonArray Sparsowane dane JSON.
     */
    static QJsonArray parseJsonArray(const QByteArray &data);
};

#endif // API_H
