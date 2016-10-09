#pragma once

#include <QStandardItemModel>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QSpacerItem>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class SMainWindow;

class SBookmarksDialog : public QDialog {
public:
    SBookmarksDialog(SMainWindow *parent = nullptr);
    ~SBookmarksDialog();

    bool loadBookMarks(QIODevice *device);
    bool saveBookMarks();

    void readBookmarksFile();

    void writeItem(QStandardItem *item);
    void readTitle(QStandardItem *item);
    void readSeparator(QStandardItem *item);
    void readFolder(QStandardItem *item);
    void readBookmark(QStandardItem *item);
private:
    QStandardItem *createChildItem(QStandardItem *item, bool havUrl = false, QString url = "");

    SMainWindow *m_parent{ nullptr };
    QStandardItemModel *m_model{ new QStandardItemModel(this) };

    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QHBoxLayout *m_layoutBoxBtn{ new QHBoxLayout() };
    QTreeView *m_view{ new QTreeView(this) };
    QSpacerItem *m_spacer{ new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum) };
    QPushButton *m_deleteBtn{ new QPushButton("Effacer", this) };
    QPushButton *m_addFolderBtn{ new QPushButton("Nouveau dossier", this) };
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };

    QXmlStreamReader m_xml{};
    QXmlStreamWriter m_stream{};
    QFile m_bookmarksFile{ "Bookmarks.xbel" };
    QIcon m_folderIcon{};
    QIcon m_itemIcon{};
};
