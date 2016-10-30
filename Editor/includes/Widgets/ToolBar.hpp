#pragma once

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QToolBar>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QStandardItemModel>
#include <QListView>
#include <QIcon>
#include <QVector>

class MainWindow;

class ToolBar : public QToolBar
{
public:
	ToolBar(MainWindow *parent = nullptr);
	~ToolBar();

	void addNewAction(QAction *action);
	void addNewWidget(QWidget *widget);
	void reset();

	void deleteThis();

	void loadToolBarV0(QTextStream & in);
	void loadToolBarV1(QTextStream & in);

	QVector<QObject*> itemInToolBar{};
	int iconSize{ 32 };
private:
	void openParams();

	MainWindow *m_parent{ nullptr };
    QAction *m_paramAction{ new QAction(QIcon(":/images/editor/setting.png"), QObject::tr("Paramètres"), this) };
    QAction *m_removeAction{ new QAction(QIcon(":/images/editor/remove.png"), QObject::tr("Supprimer la barre d'outils"), this) };
};

class ManageToolBar : public QDialog
{
public:
	ManageToolBar(ToolBar *parentToolBar, MainWindow *parent);
	~ManageToolBar();

	void newItem();
	void removeItem();

	void accept();
private:
    MainWindow *m_parent{ nullptr };
    ToolBar *m_parentToolBar{ nullptr };

	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QHBoxLayout *m_buttonLayout{ new QHBoxLayout() };

	QLabel *m_infoSize{ new QLabel(QObject::tr("Taille des icones"), this) };
	QSpinBox *m_size{ new QSpinBox(this) };
	QListView *m_view{ new QListView(this) };
	QStandardItemModel *m_model{ new QStandardItemModel(m_view) };

	QPushButton *m_newButton{ new QPushButton(QObject::tr("Nouveau"), this) };
	QPushButton *m_deleteButton{ new QPushButton(QObject::tr("Supprimer"), this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };

};

class AddItemDialog : public QDialog
{
public:
	AddItemDialog(QWidget *parent = nullptr);
	~AddItemDialog();

	void setLabelText(const QString& text);
	void setComboBoxItems(const QStringList &texts, const QList<QIcon> icons);

	QString textValue();
	static QString getItem(QWidget *parent, const QString &title, const QString &label, const QStringList &texts, const QList<QIcon> icons, bool *ok);
private:
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QLabel *m_label{ new QLabel(this) };
	QComboBox *m_items{ new QComboBox(this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this) };
};
