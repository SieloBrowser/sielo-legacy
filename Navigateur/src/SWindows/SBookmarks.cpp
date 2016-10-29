#include "includes/SWindows/SBookmarks.hpp"
#include "includes/SMainWindow.hpp"

#include <QMessageBox>
#include <QFileInfo>
#include <QHeaderView>
#include <QStringListModel>

SBookmarksView::SBookmarksView(QWidget *parent, bool isItemEditable) :
	QTreeView(parent),
	m_isItemEditable(isItemEditable)
{
	m_bookmarksFile.setFileName(SMainWindow::dataPath + "Bookmarks.xbel");
	QStringList labels{};
	labels << tr("Title") << tr("Location");
	setSelectionMode(QTreeView::ExtendedSelection);
	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);

	if(!m_isItemEditable)
		setEditTriggers(EditTrigger::NoEditTriggers);

	// Set properties of the view
	setAutoScroll(true);
	setProperty("showDropIndicator", QVariant(false));
	setAlternatingRowColors(true);
	setAnimated(true);
	header()->setDefaultSectionSize(300);
	setModel(m_model);
	m_model->setHorizontalHeaderLabels(labels);

	// Create the icon based on the OS
	m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
	m_folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
	m_itemIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

	// Attemp to open the XBEL file
	if(!m_bookmarksFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de l'ouverture des favoris."));
		return;
	}

	if(!loadBookMarks(&m_bookmarksFile)) {
		QMessageBox::critical(this, tr("Erreur"), tr("Erreur lors de la lecture des favoris."));
		return;
	}

	m_bookmarksFile.close();
}

SBookmarksView::~SBookmarksView()
{
	// Empty
}

bool SBookmarksView::loadBookMarks(QIODevice *device)
{
	m_xml.setDevice(device);

	// Check if the file is write in good version
	if (m_xml.readNextStartElement()) {
		if (m_xml.name() == "xbel" && m_xml.attributes().value("version") == "1.0")
			readBookmarksFile();
		else
			m_xml.raiseError(tr("Le fichier n'est pas un fichier XBEL 1.0 valide."));
	}

	return !m_xml.error();

}

bool SBookmarksView::saveBookMarks()
{
	// Attempt to open the file
	if(!m_bookmarksFile.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	// Set attributes for the stream
	m_stream.setDevice(&m_bookmarksFile);
	m_stream.setAutoFormatting(true);

	// Write firth item
	m_stream.writeDTD("<!DOCTYPE xbel>");
	m_stream.writeStartElement("xbel");
	m_stream.writeAttribute("version", "1.0");

	// Write bookmarks, folder and separator
	for(int i{ 0 }; i < m_model->rowCount(); ++i)
		writeItem(m_model->item(i));

	// Write the end of the document
	m_stream.writeEndDocument();

	m_bookmarksFile.close();
	static_cast<QWidget*>(parent())->close();
	return true;
}

void SBookmarksView::readBookmarksFile()
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

void SBookmarksView::writeItem(QStandardItem *item)
{
	QString tagName{ item->data(Qt::UserRole).toString() };

	if(tagName == "folder") {
		m_stream.writeStartElement(tagName);
		m_stream.writeAttribute("folded", item->parent() ? "yes" : "no");
		m_stream.writeTextElement("title", item->text());
		for(int i{ 0 }; i < item->rowCount(); ++i)
			writeItem(item->child(i));
		m_stream.writeEndElement();
	}
	else if(tagName == "bookmark") {
		m_stream.writeStartElement(tagName);
		if (!m_model->data(item->parent()->child(item->row(), 1)->index()).toString().isEmpty())
			m_stream.writeAttribute("href", m_model->data(item->parent()->child(item->row(), 1)->index()).toString());
		m_stream.writeTextElement("title", item->text());
		m_stream.writeEndElement();

		QFile icon{ SMainWindow::dataPath + "Images/FavIcons/icon" + item->text() + ".png" };
		if (!icon.open(QIODevice::WriteOnly))
			return;
		item->icon().pixmap(64, 64).save(&icon, "PNG");
		icon.close();
	}
	else if(tagName == "separator") {
		m_stream.writeEmptyElement(tagName);
	}
}

void SBookmarksView::readTitle(QStandardItem *item)
{
	item->setText(m_xml.readElementText());
	if(item->data(Qt::UserRole).toString() == "bookmark")
		item->setIcon(QIcon(SMainWindow::dataPath + "Images/FavIcons/icon" + item->text() + ".png"));
}

void SBookmarksView::readSeparator(QStandardItem *item)
{
	QStandardItem *separator = createChildItem(item, true, QString(30, 0xB7));
	separator->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
	item->child(separator->row(), 1)->setFlags(item->flags() & ~Qt::ItemIsSelectable & ~Qt::ItemIsEditable);
	separator->setText(QString(30, 0xB7));
	m_xml.skipCurrentElement();
}

void SBookmarksView::readFolder(QStandardItem *item)
{
	QStandardItem *folder = createChildItem(item);
	bool folded = (m_xml.attributes().value("folded") != "no");

	folder->setIcon(m_folderIcon);

	if(!folded)
		expand(folder->index());

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

void SBookmarksView::readBookmark(QStandardItem *item)
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

QStandardItem *SBookmarksView::createChildItem(QStandardItem *item, bool havUrl, QString url)
{
	QList<QStandardItem*> items{};
	items.append(new QStandardItem{});
	if (item) {
		if(havUrl)
			items.append(new QStandardItem(url));

		item->appendRow(items);
	}
	else {
		if(havUrl)
			items.append(new QStandardItem(url));

		m_model->appendRow(items);
	}
	items[0]->setData(m_xml.name().toString(), Qt::UserRole);
	return items[0];
}


void SBookmarksView::dragMoveEvent(QDragMoveEvent * event)
{
	if (m_model->itemFromIndex(indexAt(event->pos())) == nullptr || 
		m_model->itemFromIndex(indexAt(event->pos()))->data(Qt::UserRole).toString() == "bookmark" ||
		m_model->itemFromIndex(indexAt(event->pos()))->data(Qt::UserRole).toString() == "separator")
		
		event->ignore();
	else
		event->accept();
}

void SBookmarksView::dropEvent(QDropEvent * event)
{
	if (m_model->itemFromIndex(indexAt(event->pos())) == nullptr ||
		m_model->itemFromIndex(indexAt(event->pos()))->data(Qt::UserRole).toString() == "bookmark" ||
		m_model->itemFromIndex(indexAt(event->pos()))->data(Qt::UserRole).toString() == "separator")
		event->ignore();
	else
		QTreeView::dropEvent(event);
}

Qt::DropActions QStandardItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

SBookmarksAddDialog::SBookmarksAddDialog(SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent)
{
	// Set attributes and text for all widgets
	setModal(true);
	setAttribute(Qt::WA_DeleteOnClose);
	m_label->setText("Dossier : ");
	m_bookmarkName->setPlaceholderText(tr("Nom du favorie"));
	m_bookmarkName->setText(m_parent->currentPage()->title());
	m_view->hide();
	fillFolderBox();

	// Fill the layouts
	QHBoxLayout *folderLayout{ new QHBoxLayout() };
	folderLayout->addWidget(m_label);
	folderLayout->addWidget(m_location);

	m_layout->addWidget(m_bookmarkName);
	m_layout->addLayout(folderLayout);
	m_layout->addWidget(m_boxBtn);

	// Connection
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SBookmarksAddDialog::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SBookmarksAddDialog::close);
}

SBookmarksAddDialog::~SBookmarksAddDialog()
{

}


void SBookmarksAddDialog::fillFolderBox()
{
	// Fill the list of folder
	for(int i{ 0 }; i < m_view->getModel()->rowCount(); ++i) {
		addItemToBox(m_view->getModel()->item(i));
	}
}

void SBookmarksAddDialog::addItemToBox(QStandardItem *item)
{
	QString tagName{ item->data(Qt::UserRole).toString() };

	if(tagName == "folder") {
		m_location->addItem(item->text(), item->index());
		for(int i{ 0 }; i < item->rowCount(); ++i)
			addItemToBox(item->child(i));

	}
}

void SBookmarksAddDialog::accept()
{
	QStandardItem *item{ m_view->createChildItem(m_view->getModel()->itemFromIndex(m_location->currentData().toModelIndex()), true, m_parent->currentPage()->url().toString()) };
	item->setText(m_bookmarkName->text());
	item->setIcon(m_parent->currentPage()->icon());
	item->setData("bookmark", Qt::UserRole);
	m_view->saveBookMarks();

	// Refresh the bookmarks menu
	m_parent->getMenus()[SMenuType::Fav]->clear();
	m_parent->getMenus()[SMenuType::Fav]->createBookmarksMenu();

	close();
}

SBookmarksDialog::SBookmarksDialog(SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent)
{
	// Set attributes of widgets
	setAttribute(Qt::WA_DeleteOnClose);
	resize(758, 450);
	m_openButton->setEnabled(false);

	// Add widgets on all layouts
	m_layoutBoxBtn->addWidget(m_openButton);
	m_layoutBoxBtn->addWidget(m_deleteBtn);
	m_layoutBoxBtn->addWidget(m_addFolderBtn);
	m_layoutBoxBtn->addItem(m_spacer);
	m_layoutBoxBtn->addWidget(m_boxBtn);
	m_layout->addWidget(m_view);
	m_layout->addLayout(m_layoutBoxBtn);

	// Connection
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SBookmarksDialog::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SBookmarksDialog::close);
	connect(m_view, &SBookmarksView::pressed, this, &SBookmarksDialog::itemSelected);
	connect(m_openButton, &QPushButton::clicked, this, &SBookmarksDialog::openBoomark);
	connect(m_deleteBtn, &QPushButton::clicked, this, &SBookmarksDialog::deleteBookmark);
	connect(m_addFolderBtn, &QPushButton::clicked, this, &SBookmarksDialog::addFolder);

}

SBookmarksDialog::~SBookmarksDialog()
{
	// Empty
}

void SBookmarksDialog::openBoomark()
{
	// Variables
	QModelIndex index{ m_view->currentIndex() };
	QString title{};
	QUrl url{};

	if(index.column() == 1) {
		title = m_view->getModel()->data(m_view->getModel()->itemFromIndex(index)->parent()->child(index.row())->index()).toString();
		url = m_view->getModel()->data(index).toUrl();
	}
	else {
		title = m_view->getModel()->data(index).toString();
		url = m_view->getModel()->data(m_view->getModel()->itemFromIndex(index)->parent()->child(index.row(), 1)->index()).toUrl();
	}

	m_parent->getTabs()->createWebTab(title, url);
	m_parent->getTabs()->createPlusTab();
	m_parent->getTabs()->removeTab(m_parent->getTabs()->count() - 3);
	m_parent->getTabs()->setCurrentIndex(m_parent->getTabs()->count() - 2);
}

void SBookmarksDialog::deleteBookmark()
{
	// Check if it's a folder or not
	if(m_view->getModel()->itemFromIndex(m_view->currentIndex())->parent())
		m_view->getModel()->itemFromIndex(m_view->currentIndex())->parent()->removeRow(m_view->getModel()->itemFromIndex(m_view->currentIndex())->row());
	else
		m_view->getModel()->removeRow(m_view->getModel()->itemFromIndex(m_view->currentIndex())->row());
}

void SBookmarksDialog::addFolder()
{
	QStandardItem *folder{ nullptr };

	if(!m_view->getModel()->itemFromIndex(m_view->currentIndex())->parent())
		folder = m_view->createChildItem(m_view->getModel()->itemFromIndex(m_view->currentIndex()));
	else if(m_view->getModel()->itemFromIndex(m_view->currentIndex())->parent() && m_view->getModel()->itemFromIndex(m_view->currentIndex())->data(Qt::UserRole).toString() == "folder")
		folder = m_view->createChildItem(m_view->getModel()->itemFromIndex(m_view->currentIndex()));
	else
		folder = m_view->createChildItem(m_view->getModel()->itemFromIndex(m_view->currentIndex())->parent());

	folder->setText("Nouveau dossier");
	folder->setIcon(m_view->getFolderIcon());
	folder->setData("folder", Qt::UserRole);
}

void SBookmarksDialog::accept()
{
	m_view->saveBookMarks(); //< Save bookmarks

	// Refresh the bookmarks menu
	m_parent->getMenus()[SMenuType::Fav]->clear();
	m_parent->getMenus()[SMenuType::Fav]->createBookmarksMenu();
}

void SBookmarksDialog::itemSelected(const QModelIndex &index)
{
	QStandardItem *item{ m_view->getModel()->itemFromIndex(index) };
	if(item->data(Qt::UserRole).toString() == "bookmark")
		m_openButton->setEnabled(true);
	else
		m_openButton->setEnabled(false);
}
