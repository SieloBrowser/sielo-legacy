#include "includes/Widgets/ToolBar.hpp"
#include "includes/MainWindow.hpp"

#include <QInputDialog>
#include <QMessageBox>
#include <QAction>
#include <QString>
#include <QTextStream>

ToolBar::ToolBar(MainWindow * parent) : 
	QToolBar(parent),
	m_parent(parent)
{
	setFloatable(false);

	addAction(m_paramAction);
	addAction(m_removeAction);
	addSeparator();
	addSeparator();

	connect(m_paramAction, &QAction::triggered, this, &ToolBar::openParams);
	connect(m_removeAction, &QAction::triggered, this, &ToolBar::deleteThis);
}

ToolBar::~ToolBar()
{
	// Empty
}

void ToolBar::addNewAction(QAction *action)
{
	itemInToolBar.push_back(action);
	addAction(action);
}

void ToolBar::addNewWidget(QWidget * widget)
{
	widget->setParent(m_parent);
	itemInToolBar.push_back(widget);
	addWidget(widget);
}

void ToolBar::reset()
{
	for (int i{ 0 }; i < itemInToolBar.size(); i++) {
		if (itemInToolBar[i]->objectName() == "urlArea")
			m_parent->m_urlAreas.remove(m_parent->m_urlAreas.indexOf(static_cast<QLineEdit*>(itemInToolBar[i])));
		else if (itemInToolBar[i]->objectName() == "searchArea")
			m_parent->m_searchAreas.remove(m_parent->m_searchAreas.indexOf(static_cast<QLineEdit*>(itemInToolBar[i])));
		else if (itemInToolBar[i]->objectName() == "spacer")
			m_parent->m_spacers.remove(m_parent->m_spacers.indexOf(static_cast<QWidget*>(itemInToolBar[i])));
	}
	itemInToolBar.clear();
	clear();

	addAction(m_paramAction);
	addAction(m_removeAction);
	addSeparator();
	addSeparator();

}

void ToolBar::deleteThis()
{
	m_parent->thmSaved = false;
	m_parent->deleteToolBar(this);
}

void ToolBar::loadToolBarV0(QTextStream & in)
{

}

void ToolBar::loadToolBarV1(QTextStream & in)
{
	unsigned widgetInToolBar{ 0 };
	QString currentWidget{ "nothing" };
	QString toolBarPos{ "top" };

	in >> toolBarPos; //< Tool bar position in the window
	in >> iconSize;
	in >> widgetInToolBar; //< Number of widgets in the tool bar from the tooBar.txt file

	if (toolBarPos == "left")
		m_parent->addToolBar(Qt::LeftToolBarArea, this);
	else if (toolBarPos == "right")
		m_parent->addToolBar(Qt::RightToolBarArea, this);
	else if (toolBarPos == "bottom")
		m_parent->addToolBar(Qt::BottomToolBarArea, this);
	else
		m_parent->addToolBar(this);

	setIconSize(QSize(iconSize, iconSize));

	for (size_t i{ 0 }; i < widgetInToolBar; ++i) {
		in >> currentWidget;
		if (currentWidget == "back")
			addNewAction(m_parent->m_backAction);
		else if (currentWidget == "next")
			addNewAction(m_parent->m_nextAction);
		else if (currentWidget == "home")
			addNewAction(m_parent->m_homeAction);
		else if (currentWidget == "refresh")
			addNewAction(m_parent->m_refreshOrStopAction);
		else if (currentWidget == "go")
			addNewAction(m_parent->m_goAction);
		else if (currentWidget == "search")
			addNewAction(m_parent->m_searchAction);
		else if (currentWidget == "history")
			addNewAction(m_parent->m_sowHistory);
		else if (currentWidget == "preferences")
			addNewAction(m_parent->m_preferencesAction);
		else if (currentWidget == "addBookmarks")
			addNewAction(m_parent->m_addBookmarksAction);
		else if (currentWidget == "bookmarksManager")
			addNewAction(m_parent->m_bookmarsManagerAction);
		else if (currentWidget == "newTab")
			addNewAction(m_parent->m_newTabAction);
		else if (currentWidget == "newWin")
			addNewAction(m_parent->m_newWindowAction);
		else if (currentWidget == "urlArea") {
			QLineEdit *newUrlArea{ new QLineEdit(m_parent) };
			newUrlArea->setPlaceholderText(QObject::tr("Ici l'url de la page"));
			newUrlArea->setObjectName("urlArea");
			m_parent->m_urlAreas.push_back(newUrlArea);
			addNewWidget(newUrlArea);
		}
		else if (currentWidget == "searchArea") {
			QLineEdit *newSearchArea{ new QLineEdit(m_parent) };
			newSearchArea->setPlaceholderText(tr("Recherche Google"));
			newSearchArea->setObjectName("searchArea");
			m_parent->m_searchAreas.push_back(newSearchArea);
			addNewWidget(newSearchArea);
		}
		else if (currentWidget == "separator") {
			addSeparator();
		}
		else if (currentWidget == "spacer") {
			QWidget *newSpacer{ new QWidget(m_parent) };
			newSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			newSpacer->setObjectName("spacer");
			m_parent->m_spacers.push_back(newSpacer);
			addNewWidget(newSpacer);
		}
		else {
			QMessageBox::warning(this, tr("Probleme"), tr("Une erreur est présente à la ligne numéro ") + QString::number(i + 1) + tr(". "
				"La barre de navigation risque de ne pas se charger comme prévu"
				"Nous vous conseillons de contacter le créateur du thème pour qu'il corrige l'erreur au plus vite."));
		}
	}
}

void ToolBar::openParams()
{
	ManageToolBar *manager{ new ManageToolBar(this, m_parent) };
	manager->show();
}

ManageToolBar::ManageToolBar(ToolBar * parentToolBar, MainWindow * parent) : 
	QDialog(parent),
	m_parent(parent),
	m_parentToolBar(parentToolBar)
{
	m_view->setAcceptDrops(true);
	m_view->setDragEnabled(true);
	m_view->setDropIndicatorShown(true);
	m_view->setModel(m_model);
	m_view->setEditTriggers(QListView::NoEditTriggers);

	m_size->setValue(m_parentToolBar->iconSize);

	for (int i{ 0 }; i < m_parentToolBar->itemInToolBar.size(); ++i) {
		QString actionName{ m_parentToolBar->itemInToolBar[i]->objectName() };
		QIcon actionIcon{};
		if (m_parentToolBar->itemInToolBar[i]->objectName() != "searchArea" || m_parentToolBar->itemInToolBar[i]->objectName() != "urlArea")
			actionIcon = static_cast<QAction*>(m_parentToolBar->itemInToolBar[i])->icon();
		m_model->appendRow(new QStandardItem(actionIcon, actionName));
	}

	m_layout->addWidget(m_infoSize);
	m_layout->addWidget(m_size);
	m_layout->addWidget(m_view);
	m_layout->addLayout(m_buttonLayout);

	m_buttonLayout->addWidget(m_newButton);
	m_buttonLayout->addWidget(m_deleteButton);
	m_buttonLayout->addWidget(m_boxBtn);

	connect(m_newButton, &QPushButton::clicked, this, &ManageToolBar::newItem);
	connect(m_deleteButton, &QPushButton::clicked, this, &ManageToolBar::removeItem);
	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &ManageToolBar::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &ManageToolBar::close);
}

ManageToolBar::~ManageToolBar()
{
	// Empty
}

void ManageToolBar::newItem()
{
	QStringList items{};
	QList<QIcon> icons{};

	QHashIterator<QString, QAction*> i{ m_parent->m_editableAction };
	while (i.hasNext())
	{
		i.next();
		QAction *action = m_parent->m_editableAction[i.key()];
		items << i.key();
		icons.append(action->icon());
	}
	items << "urlArea" << "searchArea" << "spacer";

	bool ok{ false };
	QString newAction{ AddItemDialog::getItem(this, tr("Item à ajouter"), tr("iteme :"), items, icons, &ok) };

	if (ok && !newAction.isEmpty()) {
		if (newAction != "urlArea" &&
			newAction != "searchArea" &&
			newAction != "spacer") {
			QString actionName{ m_parent->m_editableAction[newAction]->objectName() };
			QIcon actionIcon{ m_parent->m_editableAction[newAction]->icon() };
			m_model->appendRow(new QStandardItem(actionIcon, actionName));
		}
		else if (newAction == "urlArea")
			m_model->appendRow(new QStandardItem("urlArea"));
		else if (newAction == "searchArea")
			m_model->appendRow(new QStandardItem("searchArea"));
		else if (newAction == "spacer")
			m_model->appendRow(new QStandardItem("spacer"));
	}
}

void ManageToolBar::removeItem()
{
	QModelIndex index{ m_view->currentIndex() };
	m_model->removeRow(index.row());
}

void ManageToolBar::accept()
{
	m_parentToolBar->reset();

	for (int i{ 0 }; i < m_model->rowCount(); ++i) {
		if (m_model->item(i)->text() != "urlArea" &&
			m_model->item(i)->text() != "searchArea" &&
			m_model->item(i)->text() != "spacer")
			m_parentToolBar->addNewAction(m_parent->m_editableAction[m_model->item(i)->text()]);
		else if (m_model->item(i)->text() == "urlArea") {
			QLineEdit *newUrlArea{ new QLineEdit(m_parent) };
			newUrlArea->setPlaceholderText(QObject::tr("Ici l'url de la page"));
			newUrlArea->setObjectName("urlArea");
			m_parent->m_urlAreas.push_back(newUrlArea);
			m_parentToolBar->addNewWidget(newUrlArea);
		}
		else if (m_model->item(i)->text() == "searchArea") {
			QLineEdit *newSearchArea{ new QLineEdit(m_parent) };
			newSearchArea->setPlaceholderText(tr("Recherche Google"));
			newSearchArea->setObjectName("searchArea");
			m_parent->m_searchAreas.push_back(newSearchArea);
			m_parentToolBar->addNewWidget(newSearchArea);
		}
		else if (m_model->item(i)->text() == "spacer") {
			QWidget *newSpacer{ new QWidget(m_parent) };
			newSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			newSpacer->setObjectName("spacer");
			m_parent->m_spacers.push_back(newSpacer);
			m_parentToolBar->addNewWidget(newSpacer);
		}
	}

	m_parentToolBar->iconSize = m_size->value();
	m_parentToolBar->setIconSize(QSize(m_size->value(), m_size->value()));

	m_parent->thmSaved = false;

	close();
}

Qt::DropActions QStandardItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

AddItemDialog::AddItemDialog(QWidget * parent) :
	QDialog(parent)
{
	m_layout->addWidget(m_label);
	m_layout->addWidget(m_items);
	m_layout->addWidget(m_boxBtn);

	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &AddItemDialog::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &AddItemDialog::reject);
}

AddItemDialog::~AddItemDialog()
{
}

void AddItemDialog::setLabelText(const QString & text)
{
	m_label->setText(text);
}

void AddItemDialog::setComboBoxItems(const QStringList & texts, const QList<QIcon> icons)
{
	m_items->clear();
	for (int i{ 0 }; i < texts.size(); ++i) {
		if (i < icons.size())
			m_items->addItem(icons[i], texts[i]);
		else
			m_items->addItem(texts[i]);
	}
}

QString AddItemDialog::textValue()
{
	return m_items->currentText();
}

QString AddItemDialog::getItem(QWidget * parent, const QString & title, const QString & label, const QStringList & texts, const QList<QIcon> icons, bool * ok)
{
	AddItemDialog dialog(parent);
	dialog.setWindowTitle(title);
	dialog.setLabelText(label);
	dialog.setComboBoxItems(texts, icons);

	int ret = dialog.exec();

	if (ok)
		*ok = !!ret;

	if (ret) {
		return dialog.textValue();
	}
	else {
		return QString();
	}
}
