#include "includes/SWindows/SBookmarks.hpp"

#include <QMessageBox>
#include <QFileInfo>
#include <QHeaderView>

SBookmarksDialog::SBookmarksDialog(SMainWindow *parent) :
    m_parent(parent)
{
    resize(758, 450);
    setModal(true);

    QStringList labels{};
    labels << tr("Title") << tr("Location");

    m_view->setAutoScroll(true);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setProperty("showDropIndicator", QVariant(false));
    m_view->setAlternatingRowColors(true);
    m_view->setAnimated(true);
    m_view->header()->setDefaultSectionSize(300);
    m_view->setModel(m_model);
    m_model->setHorizontalHeaderLabels(labels);

    QStyle *style = m_view->style();

    m_folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    m_folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    m_itemIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));

    m_boxBtn->setStandardButtons(QDialogButtonBox::Ok);

    m_layoutBoxBtn->addWidget(m_deleteBtn);
    m_layoutBoxBtn->addWidget(m_addFolderBtn);
    m_layoutBoxBtn->addItem(m_spacer);
    m_layoutBoxBtn->addWidget(m_boxBtn);
    m_layout->addWidget(m_view);
    m_layout->addLayout(m_layoutBoxBtn);

    QFile bookmarks{ "Bookmarks.xbel" };

    if(!bookmarks.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'ouverture des favories");
        return;
    }

    if(!loadBookMarks(&bookmarks)) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la lecture des favories");
        return;
    }
}

SBookmarksDialog::~SBookmarksDialog()
{

}

bool SBookmarksDialog::loadBookMarks(QIODevice *device)
{
    m_xml.setDevice(device);

    if (m_xml.readNextStartElement()) {
        if (m_xml.name() == "xbel" && m_xml.attributes().value("version") == "1.0")
            readBookmarksFile();
        else
            m_xml.raiseError(tr("Le fichier n'est pas un fichier XBEL 1.0."));
    }

    return !m_xml.error();

}

void SBookmarksDialog::readBookmarksFile()
{
    while (m_xml.readNextStartElement()) {
        if(m_xml.name() == "folder")
            readFolder(nullptr);
        else if(m_xml.name() == "bookmark")
            readBookmark(nullptr);
        else if(m_xml.name() == "separator")
            readSeparator(nullptr);
        else
            m_xml.skipCurrentElement();
    }
}


void SBookmarksDialog::readTitle(QStandardItem *item)
{
    item->setText(m_xml.readElementText());
}

void SBookmarksDialog::readSeparator(QStandardItem *item)
{
    QStandardItem *separator = createChildItem(item);
    separator->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    separator->setText(QString(30, 0xB7));
    m_xml.skipCurrentElement();
}

void SBookmarksDialog::readFolder(QStandardItem *item)
{
    QStandardItem *folder = createChildItem(item);
    bool folded = (m_xml.attributes().value("folded") != "no");

    folder->setIcon(m_folderIcon);

    if(!folded)
        m_view->expand(folder->index());

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "title")
            readTitle(folder);
        else if (m_xml.name() == "folder")
            readFolder(folder);
        else if (m_xml.name() == "bookmark")
            readBookmark(folder);
        else if (m_xml.name() == "separator")
            readSeparator(folder);
        else
            m_xml.skipCurrentElement();
    }
}

void SBookmarksDialog::readBookmark(QStandardItem *item)
{
    QStandardItem *bookmark = createChildItem(item, true, m_xml.attributes().value("href").toString());
    bookmark->setText(tr("Titre inconnu"));
    bookmark->setIcon(m_itemIcon);

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "title")
            readTitle(bookmark);
        else
            m_xml.skipCurrentElement();
    }
}

QStandardItem *SBookmarksDialog::createChildItem(QStandardItem *item, bool havUrl, QString url)
{
    QList<QStandardItem*> items{ new QStandardItem };
    if (item) {
        if(havUrl)
            items.push_back(new QStandardItem(url));

        item->appendRow(items);
    }
    else {
        if(havUrl)
            items.push_back(new QStandardItem(url));

        m_model->appendRow(items);
    }
    return items[0];
}

