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
 */

#pragma once

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#include "SharedDefines.hpp"

class SIELO_SHAREDLIB PiwikTracker : public QObject {
    Q_OBJECT

public:
    explicit PiwikTracker(QCoreApplication * parent,
                          QUrl trackerUrl,
                          int siteId,
                          QString clientId = "");
    void sendVisit(QString path, QString actionName = "");
    void sendPing();
    void sendEvent(
            QString path,
            QString eventCategory,
            QString eventAction,
            QString eventName = "",
            int eventValue = 0);
    void setCustomDimension(int id, QString value);
    void setCustomVisitVariables(QString key, QString value);

private:
    mutable QNetworkAccessManager _networkAccessManager;
    QString _appName;
    QUrl _trackerUrl;
    int _siteId;
    QString _clientId;
    QString _screenResolution;
    QString _userAgent;
    QString _userLanguage;
    QHash<int, QString> _customDimensions;
    QHash<QString, QString> _visitVariables;
    QUrlQuery prepareUrlQuery(QString path);
    QString getVisitVariables();
private Q_SLOTS:

    void replyFinished(QNetworkReply * reply);
    void replyError(QNetworkReply::NetworkError code);
};
