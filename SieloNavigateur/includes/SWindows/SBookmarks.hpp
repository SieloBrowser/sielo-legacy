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

// This is the QTreeView for bookmarks.
class SBookmarksView : public QTreeView
{
public:
	// Constructor and destructor
	SBookmarksView(QWidget *parent = nullptr, bool isItemEditable = true);
	~SBookmarksView();

	// Function called at the start and the end
	bool loadBookMarks(QIODevice *device);
	bool saveBookMarks();

	void readBookmarksFile(); //< Function called to read the XBEl file

	void writeItem(QStandardItem *item); //< Write item in XBEL file
	void readTitle(QStandardItem *item); //< Read the title attribute in XBEL file
	void readSeparator(QStandardItem *item); //< Read separators in XBEL file
	void readFolder(QStandardItem *item); //< Read folder in XBEL file
	void readBookmark(QStandardItem *item); //< Read bookmark in XBEL file

	// Add an item in the model
	QStandardItem *createChildItem(QStandardItem *item, bool havUrl = false, QString url = ""); 

	QStandardItemModel *getModel() { return m_model; } //< Return the model of the view
	QIcon& getFolderIcon() { return m_folderIcon; } //< Return the folder icon of the OS
	QIcon& getItemIcon() { return m_itemIcon; } //< Return the file icon of the OS

protected:
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	QStandardItemModel *m_model{ new QStandardItemModel(this) };
	QXmlStreamReader m_xml{};
	QXmlStreamWriter m_stream{};
	QFile m_bookmarksFile{};
	QIcon m_folderIcon{};
	QIcon m_itemIcon{};

	bool m_isItemEditable{ true };
};

// This is the dialog called when the user want to add a bookmark
class SBookmarksAddDialog : public QDialog
{
public:
	// Constructor and destructor
	SBookmarksAddDialog(SMainWindow *parent = nullptr);
	~SBookmarksAddDialog();

	void fillFolderBox(); //< Fill the folder list
	void accept(); //< Save the bookmark
private:
	// Private functions
	void addItemToBox(QStandardItem *item);

	// Private attributes
	SMainWindow *m_parent{ nullptr };

	// Layout of the windows
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };

	// Widgets of the window
	QLabel *m_label{ new QLabel(QObject::tr("Choisissez le dossier où ajouter le favorie"), this) };
	QLineEdit *m_bookmarkName{ new QLineEdit(this) };
	QComboBox *m_location{ new QComboBox(this) };
	SBookmarksView *m_view{ new SBookmarksView(this, false) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };
};

// This is the class to show bookmarks view to the user and manage bookmarks
class SBookmarksDialog : public QDialog {
public:
	// Constructor and destructor 
	SBookmarksDialog(SMainWindow *parent = nullptr);
	~SBookmarksDialog();

public slots:
	void openBoomark();
	void deleteBookmark();
	void addFolder();

	void accept(); //< Save the bookmarks in file

	void itemSelected(const QModelIndex &index); //< Enable or disable "Open" button

private:
	SMainWindow *m_parent{ nullptr };

	// Layouts of the window
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QHBoxLayout *m_layoutBoxBtn{ new QHBoxLayout() };

	// Widgets of the window
	SBookmarksView *m_view{ new SBookmarksView(this) };

	QSpacerItem *m_spacer{ new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum) };
	QPushButton *m_openButton{ new QPushButton(QObject::tr("Ouvrir"), this) };
	QPushButton *m_deleteBtn{ new QPushButton(QObject::tr("Effacer"), this) };
	QPushButton *m_addFolderBtn{ new QPushButton(QObject::tr("Nouveau dossier"), this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };
};
