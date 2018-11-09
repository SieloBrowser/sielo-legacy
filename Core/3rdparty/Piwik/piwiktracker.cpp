/**
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * To build PiwikTracker with a QtQuick application (QGuiApplication) instead
 * of Desktop, define PIWIK_TRACKER_QTQUICK in your .pro file like this:
 * `DEFINES += PIWIK_TRACKER_QTQUICK`
 * or in a cmake project:
 * `add_definitions(-DPIWIK_TRACKER_QTQUICK)`
 *
 * To enable debugging messages, `#define PIWIK_TRACKER_DEBUG 1` before
 * including the header file
 */

#include "piwiktracker.h"
#include <QUrlQuery>
#include <QUuid>

#include "Utils/Settings.hpp"

#if defined(PIWIK_TRACKER_QTQUICK)
#include <QGuiApplication>
#include <QScreen>
#else
#ifdef QT_GUI_LIB
#include <QApplication>
#include <QDesktopWidget>
#endif
#endif

#ifndef PIWIK_TRACKER_DEBUG
#define PIWIK_TRACKER_DEBUG 0
#endif

PiwikTracker::PiwikTracker(QCoreApplication * parent,
                           QUrl trackerUrl,
                           int siteId,
                           QString clientId) :
        QObject(parent),
        _networkAccessManager(this),
        _trackerUrl(trackerUrl),
        _siteId(siteId),
        _clientId(clientId) {
    connect(
            &_networkAccessManager,
            SIGNAL(finished(QNetworkReply *)),
            this,
            SLOT(replyFinished(QNetworkReply *)));

    if (parent) {
        _appName = parent->applicationName();
    }

    // if no client id was set let's search in the settings
    if (!_clientId.size()) {
        Sn::Settings settings;
        // create a client id if none was in the settings
        if (!settings.contains("PiwikClientId")) {
            QByteArray ba;
            ba.append(QUuid::createUuid().toString());

            // generate a random md5 hash
            QString md5Hash = QString(
                    QCryptographicHash::hash(
                            ba,
                            QCryptographicHash::Md5).toHex());

            // the client id has to be a 16 character hex code
            _clientId = md5Hash.left(16);

            // store the client id
            settings.setValue("PiwikClientId", _clientId);
        } else {
            // load the client id from the settings
            _clientId = settings.value("PiwikClientId").toString();
        }
    }

    // get the screen resolution for gui apps
#if defined(PIWIK_TRACKER_QTQUICK)
    QScreen* screen = qApp->primaryScreen();
    _screenResolution = QString::number(screen->geometry().width())
        + "x" + QString::number(screen->geometry().height());
#else
#ifdef QT_GUI_LIB
    _screenResolution =
            QString::number(
                    QApplication::desktop()->screenGeometry().width()) + "x"
                    + QString::number(
                    QApplication::desktop()->screenGeometry().height());
#endif
#endif

    // try to get the operating system
    QString operatingSystem = "Other";

#ifdef Q_OS_LINUX
    operatingSystem = "Linux";
#endif

#ifdef Q_OS_MAC
    operatingSystem = "Macintosh";
#endif

#ifdef Q_OS_WIN32
    operatingSystem = "Windows";
#endif

    // for QT >= 5.4 we can use QSysInfo
    // Piwik doesn't recognize that on Mac OS X very well
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#ifdef Q_OS_MAC
    operatingSystem = "Macintosh " + QSysInfo::prettyProductName();
#else
    operatingSystem = QSysInfo::prettyProductName() +", " +
            QSysInfo::currentCpuArchitecture();
#endif
#endif

    // get the locale
    QString locale = QLocale::system().name().toLower().replace("_", "-");

    // set the user agent
    _userAgent = "Mozilla/5.0 (" + operatingSystem + "; " + locale + ") "
                      "PiwikTracker/0.1 (Qt/" QT_VERSION_STR " )";

    // set the user language
    _userLanguage = locale;
}

/**
 * Prepares the common query items for the tracking request
 */
QUrlQuery PiwikTracker::prepareUrlQuery(QString path) {
    QUrlQuery q;
    q.addQueryItem("idsite", QString::number(_siteId));
    q.addQueryItem("_id", _clientId);
    q.addQueryItem("cid", _clientId);
    q.addQueryItem("url", "http://" + _appName + "/" + path);

    // to record the request
    q.addQueryItem("rec", "1");

    // api version
    q.addQueryItem("apiv", "1");

    if (!_screenResolution.isEmpty()) {
        q.addQueryItem("res", _screenResolution);
    }

    if (!_userAgent.isEmpty()) {
        q.addQueryItem("ua", _userAgent);
    }

    if (!_userLanguage.isEmpty()) {
        q.addQueryItem("lang", _userLanguage);
    }

    if (_customDimensions.count() > 0) {
        QHash<int, QString>::iterator i;
        for (i = _customDimensions.begin(); i != _customDimensions.end(); ++i) {
            q.addQueryItem("dimension" + QString::number(i.key()), i.value());
        }
    }

    return q;
}

QString PiwikTracker::getVisitVariables()
{
    QString varString;
    /**
      * See spec at https://github.com/piwik/piwik/issues/2165
      * Need to pass in format {"1":["key1","value1"],"2":["key2","value2"]}
      */
    if( _visitVariables.count() > 0 ) {
        QHash<QString, QString>::iterator i;
        varString.append("{");
        int num=0;
        for (i = _visitVariables.begin(); i != _visitVariables.end(); ++i) {
            if( num != 0 )
            {
                varString.append(",");
            }
            QString thisvar=QString("\"%1\":[\"%2\",\"%3\"]").arg(num+1).arg(i.key()).arg(i.value());
            varString.append(thisvar);
            num++;
        }
        varString.append("}");
    }
    return varString;
}


/**
 * Sends a visit request with visit variables
 */
void PiwikTracker::sendVisit(QString path, QString actionName) {
    QUrl url(_trackerUrl.toString() + "/piwik.php");
    QUrlQuery q = prepareUrlQuery(path);
    QString visitVars=getVisitVariables();

    if( visitVars.size() != 0 )
    {
        q.addQueryItem("_cvar",visitVars);
    }
    if (!actionName.isEmpty()) {
        q.addQueryItem("action_name", actionName);
    }


    url.setQuery(q);

    // try to ensure the network is accessible
    _networkAccessManager.setNetworkAccessible(
            QNetworkAccessManager::Accessible);

    QNetworkReply *reply = _networkAccessManager.get(QNetworkRequest(url));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));

    // ignoring SSL errors
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply,
            SLOT(ignoreSslErrors()));

#if PIWIK_TRACKER_DEBUG
    qDebug() << __func__ << " - 'url': " << url;
#endif
}

/**
 * Sends a ping request
 */
void PiwikTracker::sendPing() {
    QUrl url(_trackerUrl.toString() + "/piwik.php");
    QUrlQuery q = prepareUrlQuery("");
    q.addQueryItem("ping", "1");
    url.setQuery(q);

    // try to ensure the network is accessible
    _networkAccessManager.setNetworkAccessible(
            QNetworkAccessManager::Accessible);

    QNetworkReply *reply = _networkAccessManager.get(QNetworkRequest(url));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));

    // ignoring SSL errors
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply,
            SLOT(ignoreSslErrors()));

#if PIWIK_TRACKER_DEBUG
    qDebug() << __func__ << " - 'url': " << url;
#endif
}

/**
 * Sends an event request
 */
void PiwikTracker::sendEvent(
        QString path,
        QString eventCategory,
        QString eventAction,
        QString eventName,
        int eventValue) {
    QUrl url(_trackerUrl.toString() + "/piwik.php");
    QUrlQuery q = prepareUrlQuery(path);

    if (!eventCategory.isEmpty()) {
        q.addQueryItem("e_c", eventCategory);
    }

    if (!eventAction.isEmpty()) {
        q.addQueryItem("e_a", eventAction);
    }

    if (!eventName.isEmpty()) {
        q.addQueryItem("e_n", eventName);
    }

    q.addQueryItem("e_v", QString::number(eventValue));

    url.setQuery(q);

    // try to ensure the network is accessible
    _networkAccessManager.setNetworkAccessible(
            QNetworkAccessManager::Accessible);

    QNetworkReply *reply = _networkAccessManager.get(QNetworkRequest(url));

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));

    // ignoring SSL errors
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply,
            SLOT(ignoreSslErrors()));

#if PIWIK_TRACKER_DEBUG
    qDebug() << __func__ << " - 'url': " << url;
#endif
}

/**
 * Sets a custom dimension
 */
void PiwikTracker::setCustomDimension(int id, QString value) {
    _customDimensions[id] = value;
}

/**
 * @brief PiwikTracker::setCustomVisitVariables
 * @param name The name of the custom variable to set (key)
 * @param value The value to set for this custom variable
 */
void PiwikTracker::setCustomVisitVariables(QString name, QString value) {
    _visitVariables[name]=value;
}

void PiwikTracker::replyFinished(QNetworkReply * reply) {
#if PIWIK_TRACKER_DEBUG
    qDebug() << "Reply from " << reply->url().path();
#else
    Q_UNUSED(reply);
#endif
}

void PiwikTracker::replyError(QNetworkReply::NetworkError code) {
#if PIWIK_TRACKER_DEBUG
    qDebug() << "Network error code: " << code;
#else
    Q_UNUSED(code);
#endif
}
