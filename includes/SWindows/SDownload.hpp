#pragma once

#include <QString>
#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QWebEngineDownloadItem>

class SMainWindow;

class SDownloadItem : public QDialog
{
public:
    SDownloadItem(QWebEngineDownloadItem *download, SMainWindow *parent = nullptr);
    ~SDownloadItem();

    QWebEngineDownloadItem* getDownload() { return m_download; }

public slots:
    void progress(qint64 recept, qint64 total);
    void undo();
    void open();
    void finish();

private:
    SMainWindow *m_parent{ nullptr };
    QLabel *m_label{ new QLabel(this) };
    QProgressBar *m_progressBar{ new QProgressBar(this) };
    QWebEngineDownloadItem *m_download{ nullptr };
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };

    QString m_fileName{ "Download" };
    bool m_dlCanceled{ false };
    bool m_dlFinished{ false };
};

class SDownloadWindow : public QWidget
{
public:

private:

};
