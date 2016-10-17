#pragma once

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpacerItem>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCloseEvent>

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

class MaJDialog : public QDialog
{
public:
	MaJDialog(QWidget *parent = nullptr);
	~MaJDialog();

public slots:
	void startUpdate();
	void downloadProgress(quint64 bytesReceived, quint16 bytesTotal);
	void save();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
	bool m_installingUpdate{ false };
	bool m_updateSuccess{ false };

	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGroupBox *m_box{ new QGroupBox(this) };
	QVBoxLayout *m_boxLayout{ new QVBoxLayout(m_box) };
	QHBoxLayout *m_btnLayout{ new QHBoxLayout() };

	QLabel *m_text{ new QLabel("Nouveautées", m_box) };
	QProgressBar *m_progress{ new QProgressBar(m_box) };
	QCheckBox *m_remindMaj{ new QCheckBox("Rappeler les mises a joures", this) };
	QPushButton *m_installButton{ new QPushButton("Installer", this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };

	QNetworkReply *m_reply{};
	QNetworkAccessManager m_netManager{};
};

class SStarter : public QObject
{
public:
    SStarter(QObject *parent = nullptr);
    ~SStarter();

private:
    QNetworkReply *m_reply{};
    QString m_currentVersion{ "0.0.1b\n" };
    QString m_version{ "0.0.1b\n"};
};

