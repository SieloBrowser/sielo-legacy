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
	SHistoryWindow(SMainWindow* parent = nullptr);
	~SHistoryWindow();

private:
	SMainWindow* m_parent{ nullptr };

	QVBoxLayout* m_layout{ new QVBoxLayout(this) };
	QHBoxLayout* m_btnLayout{ new QHBoxLayout(this) };
	QSpacerItem* m_hSpacer{ new QSpacerItem(40, 20, QSizePolicy::Expanding) };
	QTreeView* m_view{ new QTreeView(this) };
	QStandardItemModel* m_model{ new QStandardItemModel(this) };
	QPushButton* m_loadBtn{ new QPushButton("Charger", this) };
	QPushButton* m_deleteAllBtn{ new QPushButton("Effacer tout l'historique", this) };
	QDialogButtonBox* m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
};