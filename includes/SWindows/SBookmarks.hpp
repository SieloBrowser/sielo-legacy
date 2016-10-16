#pragma once

#include <QStandardItemModel>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QSpacerItem>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class SMainWindow;

class SBookmarksView : public QTreeView
{
public:
    SBookmarksView(QWidget *parent = nullptr, bool isItemEditable = true);
    ~SBookmarksView();

    bool loadBookMarks(QIODevice *device);
    bool saveBookMarks();

    void readBookmarksFile();

    void writeItem(QStandardItem *item);
    void readTitle(QStandardItem *item);
    void readSeparator(QStandardItem *item);
    void readFolder(QStandardItem *item);
    void readBookmark(QStandardItem *item);

    QStandardItem *createChildItem(QStandardItem *item, bool havUrl = false, QString url = "");

    QStandardItemModel *getModel() { return m_model; }
    QIcon& getFolderIcon() { return m_folderIcon; }
    QIcon& getItemIcon() { return m_itemIcon; }

private:
    QStandardItemModel *m_model{ new QStandardItemModel(this) };
    QXmlStreamReader m_xml{};
    QXmlStreamWriter m_stream{};
    QFile m_bookmarksFile{};
    QIcon m_folderIcon{};
    QIcon m_itemIcon{};

    bool m_isItemEditable{ true };
};

class SBookmarksAddDialog : public QDialog
{
public:
    SBookmarksAddDialog(SMainWindow *parent = nullptr);
    ~SBookmarksAddDialog();

    void fillFolderBox();
    void accept();
private:
    void addItemToBox(QStandardItem *item);

    SMainWindow *m_parent{ nullptr };

    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QLabel *m_label{ new QLabel("Choisissez le dossier où ajouter le favorie", this) };
    QLineEdit *m_bookmarkName{ new QLineEdit(this) };
    QComboBox *m_location{ new QComboBox(this) };
    SBookmarksView *m_view{ new SBookmarksView(this, false) };
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
};

class SBookmarksDialog : public QDialog {
public:
    SBookmarksDialog(SMainWindow *parent = nullptr);
    ~SBookmarksDialog();

public slots:
    void openBoomark();
    void deleteBookmark();
    void addFolder();

    void accept();

    void itemSelected(const QModelIndex &index);

private:
    SMainWindow *m_parent{ nullptr };

    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QHBoxLayout *m_layoutBoxBtn{ new QHBoxLayout() };
    SBookmarksView *m_view{ new SBookmarksView(this) };
    QSpacerItem *m_spacer{ new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum) };
    QPushButton *m_openButton{ new QPushButton("Ouvrir", this) };
    QPushButton *m_deleteBtn{ new QPushButton("Effacer", this) };
    QPushButton *m_addFolderBtn{ new QPushButton("Nouveau dossier", this) };
    QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
};
