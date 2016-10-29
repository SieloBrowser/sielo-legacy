#pragma once

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QUrl>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QPushButton>
#include <QDialogButtonBox>

class SMainWindow;

struct SHistoryItem {
	QString title;
	QUrl url;
};

class SHistoryWindow : public QDialog
{
public:
	// Constructor and destructor
	SHistoryWindow(SMainWindow* parent = nullptr);
	~SHistoryWindow();

public slots:
	void load(); //< Load an history item
	void deleteOne(); //< delete an history item
	void deleteAll(); //< delete all history

private:
	// Custom delete functions
	void deleteOneFromParent();
	void deleteOneFromItem();

	SMainWindow* m_parent{ nullptr };

	// Layouts of the window
	QVBoxLayout* m_layout{ new QVBoxLayout(this) };
	QHBoxLayout* m_btnLayout{ new QHBoxLayout() };

	// Widgets of the window
	QSpacerItem* m_hSpacer{ new QSpacerItem(40, 20, QSizePolicy::Expanding) };
	QTreeView* m_view{ new QTreeView(this) };
	QStandardItemModel* m_model{ new QStandardItemModel(this) };
	QPushButton* m_deleteOneBtn{ new QPushButton(QObject::tr("Effacer"), this) };
	QPushButton* m_deleteAllBtn{ new QPushButton(QObject::tr("Effacer tout l'historique"), this) };
	QDialogButtonBox* m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this) };
};
