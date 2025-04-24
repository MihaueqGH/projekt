#include "api.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QUrl>
#include <QDebug>

/// Globalny menedżer zapytań sieciowych
QNetworkAccessManager *manager = new QNetworkAccessManager();

/**
 * @brief Wysyła żądanie HTTP pod podany URL i zwraca dane w postaci QJsonArray.
 *
 * @param urlString Adres URL, z którego mają zostać pobrane dane.
 * @param callback Funkcja wywoływana po pomyślnym pobraniu danych.
 * @param errorCallback Funkcja wywoływana w przypadku błędu (np. brak połączenia, zły URL).
 */
void ApiManager::fetchData(const QString &urlString,
                           std::function<void(const QJsonArray&)> callback,
                           std::function<void(int)> errorCallback)
{
    QUrl url(urlString);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    QObject::connect(reply, &QNetworkReply::finished,
                     [reply, callback = std::move(callback), errorCallback = std::move(errorCallback)]() mutable {
                         if (reply->error() == QNetworkReply::NoError) {
                             QByteArray responseData = reply->readAll();
                             QJsonArray data = ApiManager::parseJsonArray(responseData);
                             if (callback) {
                                 callback(data);
                             }
                         } else {
                             qDebug() << "Błąd pobierania danych:" << reply->errorString();
                             if (errorCallback) {
                                 errorCallback(reply->error());
                             }
                         }
                         reply->deleteLater();
                     });

    qDebug() << "Wysłano zapytanie HTTP na adres:" << url.toString();
}

/**
 * @brief Parsuje dane w formacie JSON i zwraca je jako QJsonArray.
 *
 * @param data Surowe dane w formacie JSON.
 * @return QJsonArray Obiekt tablicy JSON (lub pusty, jeśli wystąpił błąd parsowania).
 */
QJsonArray ApiManager::parseJsonArray(const QByteArray &data)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Błąd parsowania JSON:" << parseError.errorString();
        return QJsonArray();
    }

    if (!jsonDoc.isArray()) {
        qDebug() << "JSON nie jest tablicą!";
        return QJsonArray();
    }

    return jsonDoc.array();
}

/**
 * @brief Pobiera dane z API i przekazuje je jako QJsonDocument.
 *
 * @param url Adres URL do pobrania danych.
 * @param callback Funkcja wywoływana po pomyślnym pobraniu danych jako QJsonDocument.
 * @param errorCallback Funkcja wywoływana w przypadku błędu.
 */
void ApiManager::fetchDataRaw(const QString &url,
                              std::function<void(const QJsonDocument &)> callback,
                              std::function<void(int)> errorCallback)
{
    QNetworkRequest request((QUrl(url)));
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QObject::connect(manager, &QNetworkAccessManager::finished,
                     [manager, callback, errorCallback](QNetworkReply *reply) {
                         if (reply->error() == QNetworkReply::NoError) {
                             QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                             callback(doc);
                         } else {
                             qWarning() << "Błąd API (raw):" << reply->errorString();
                             if (errorCallback) {
                                 errorCallback(reply->error());
                             }
                         }
                         reply->deleteLater();
                         manager->deleteLater();
                     });

    manager->get(request);
}
