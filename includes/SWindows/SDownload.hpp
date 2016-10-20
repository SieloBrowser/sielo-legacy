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

// This is the download item
class SDownloadItem : public QDialog
{
public:
	// Constructor and destructor
    SDownloadItem(QWebEngineDownloadItem *download, SMainWindow *parent = nullptr);
    ~SDownloadItem();

    QWebEngineDownloadItem* getDownload() { return m_download; } //< Return the item give by the web engine

public slots:
    void progress(qint64 recept, qint64 total); //< Update progress bar
    void undo(); //< Cancel the download
    void open(); //< Open the file downloaded
    void finish(); //< Called when the download is finished 

private:
    SMainWindow *m_parent{ nullptr };

	// Widgets in the dialog
    QLabel *m_label{ new QLabel(this) };
    QProgressBar *m_progressBar{ new QProgressBar(this) };
    QWebEngineDownloadItem *m_download{ nullptr };
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };

	// Utile variable
    QString m_fileName{ "Download" };
    bool m_dlCanceled{ false };
    bool m_dlFinished{ false };
};

// TODO: Add a window for manager download
class SDownloadWindow : public QWidget
{
public:

private:

};
