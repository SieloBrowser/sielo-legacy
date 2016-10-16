#pragma once

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class TextToShow : public QDialog
{
public:
    TextToShow(QWidget *parent = nullptr);
    ~TextToShow();

private:
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QGroupBox *m_box{ new QGroupBox(this) };
    QVBoxLayout *m_boxLayout{ new QVBoxLayout(m_box) };

    QLabel *m_textToShow{ new QLabel("Texte", this)};
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };

    QNetworkReply *m_reply{};
};

class SStarter : public QObject
{
public:
    SStarter(QObject *parent = nullptr);
    ~SStarter();

private:
    QNetworkReply *m_reply{};
    QString m_currentVersion{ "0.1b\n" };
    QString m_version{ "0.1b\n"};
};

