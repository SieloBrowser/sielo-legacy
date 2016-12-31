#include "includes/SWidgets/SToolBar.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

SToolBar::SToolBar(const QString& toolBarName, SMainWindow* parent) :
	QToolBar(toolBarName),
	m_parent(parent),
	m_actions(parent->getActions())
{
	// Center widgets in the tool bar
	m_spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


SToolBar::~SToolBar()
{
}

void SToolBar::loadToolBarV0(QTextStream & in)
{
	unsigned widgetInToolBar{ 0 };
	QString currentWidget{ "nothing" };

	addWidget(m_spacer1);

	// Add widgets in the tool bar from the tooBar.txt file
	in >> widgetInToolBar;
	for (size_t i{ 0 }; i < widgetInToolBar; ++i) {
		in >> currentWidget;
		if (currentWidget == "back") 
			addAction(m_actions->back);
		else if (currentWidget == "next") 
			addAction(m_actions->next);
		else if (currentWidget == "home") 
			addAction(m_actions->home);
		else if (currentWidget == "refresh") 
			addAction(m_actions->refreshOrStop);
		else if (currentWidget == "go") 
			addAction(m_actions->go);
		else if (currentWidget == "search") 
			addAction(m_actions->shearch);
		else if (currentWidget == "history") 
			addAction(m_actions->history);
		else if (currentWidget == "preferences") 
			addAction(m_actions->preferences);
		else if (currentWidget == "addBookmarks") 
			addAction(m_actions->addBookmarks);
		else if (currentWidget == "bookmarksManager") 
			addAction(m_actions->bookmarksManager);
		else if (currentWidget == "urlArea") 
			addWidget(m_parent->getUrlArea());
		else if (currentWidget == "searchArea") 
			addWidget(m_parent->getSearchArea());
		else if (currentWidget == "separator") 
			addSeparator();
		else {
			QMessageBox::warning(this, tr("Probleme"), tr("Une erreur est présente à la ligne numéro ") + QString::number(i + 1) + tr(". " 
														  "La barre de navigation risque de ne pas se charger comme prévu"  
														  "Nous vous conseillons de contacter le créateur du thème pour qu'il corrige l'erreur au plus vite."));
		}
	}

	addWidget(m_spacer2);

}

void SToolBar::loadToolBarV1(QTextStream & in)
{
	//setStyleSheet("border: none;");
	QColor winFrameColor{ m_parent->getWindowFrameColor() };
	setStyleSheet("QToolBar {background: " + winFrameColor.name() + "; border: none;}");

	unsigned widgetInToolBar{ 0 };
	unsigned iconSize{ 32 };
	QString currentWidget{ "nothing" };
	QString toolBarPos{ "top" };

	this->setContentsMargins(0, 0, 0, 5);
		
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
			addAction(m_actions->back);
		else if (currentWidget == "next") 
			addAction(m_actions->next);
		else if (currentWidget == "home") 
			addAction(m_actions->home);
		else if (currentWidget == "refresh") 
			addAction(m_actions->refreshOrStop);
		else if (currentWidget == "go") 
			addAction(m_actions->go);
		else if (currentWidget == "search") 
			addAction(m_actions->shearch);
		else if (currentWidget == "history") 
			addAction(m_actions->history);
		else if (currentWidget == "preferences") 
			addAction(m_actions->preferences);
		else if (currentWidget == "addBookmarks") 
			addAction(m_actions->addBookmarks);
		else if (currentWidget == "bookmarksManager") 
			addAction(m_actions->bookmarksManager);
		else if (currentWidget == "newTab")
			addAction(m_actions->newTab);
        else if (currentWidget == "newWindow")
			addAction(m_actions->newWindow);
		else if (currentWidget == "urlArea") {
			addWidget(m_parent->getUrlArea());
		}
		else if (currentWidget == "searchArea") {
			addWidget(m_parent->getSearchArea());
		}
		else if (currentWidget == "separator") {
			addSeparator();
		}
		else if (currentWidget == "spacer") {
			QWidget *m_spacer{ new QWidget(this) };
			m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			addWidget(m_spacer);
		}
		else {
			QMessageBox::warning(this, tr("Probleme"), tr("Une erreur est présente à la ligne numéro ") + QString::number(i + 1) + tr(". " 
														  "La barre de navigation risque de ne pas se charger comme prévu"  
														  "Nous vous conseillons de contacter le créateur du thème pour qu'il corrige l'erreur au plus vite."));
		}
	}
}

void SToolBar::loadToolBarV2(QXmlStreamReader * fielo)
{
	QString toolBarPos{ fielo->attributes().value("position").toString() };
	QColor toolBarColor{ fielo->attributes().value("color").toString() };
	QSize toolBarSize{ fielo->attributes().value("size").toInt(), fielo->attributes().value("size").toInt() };

	if (fielo->attributes().value("color").toString() == "default")
		toolBarColor = QColor("#f0f0f0");
	else if (fielo->attributes().value("color").toString() == "system")
		toolBarColor = m_parent->getWindowFrameColor();


	if (toolBarPos == "left")
		m_parent->addToolBar(Qt::LeftToolBarArea, this);
	else if (toolBarPos == "right")
		m_parent->addToolBar(Qt::RightToolBarArea, this);
	else if (toolBarPos == "bottom")
		m_parent->addToolBar(Qt::BottomToolBarArea, this);
	else
		m_parent->addToolBar(this);

	this->setStyleSheet("QToolBar{ background: " + toolBarColor.name() + "; border: none; }");
	setIconSize(toolBarSize);
	QString name = fielo->name().toString();
	
	while (fielo->readNextStartElement()) {
		if (fielo->name() == "icon") {
			loadCurrentWidget(fielo->attributes().value("type").toString());
			fielo->skipCurrentElement();
		}
		else if (fielo->name() == "url-area") {
			addWidget(m_parent->getUrlArea());
			fielo->skipCurrentElement();
		}
		else if (fielo->name() == "search-area") {
			addWidget(m_parent->getSearchArea());
			fielo->skipCurrentElement();
		}
		else if (fielo->name() == "separator") {
			addSeparator();
			fielo->skipCurrentElement();
		}
		else if (fielo->name() == "spacer") {
			QWidget *m_spacer{ new QWidget(this) };
			m_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			addWidget(m_spacer);
			fielo->skipCurrentElement();
		}
	}
}

void SToolBar::loadCurrentWidget(const QString &currentWidget)
{
	if (currentWidget == "back")
		addAction(m_actions->back);
	else if (currentWidget == "next")
		addAction(m_actions->next);
	else if (currentWidget == "home")
		addAction(m_actions->home);
	else if (currentWidget == "refresh")
		addAction(m_actions->refreshOrStop);
	else if (currentWidget == "go")
		addAction(m_actions->go);
	else if (currentWidget == "search")
		addAction(m_actions->shearch);
	else if (currentWidget == "history")
		addAction(m_actions->history);
	else if (currentWidget == "preferences")
		addAction(m_actions->preferences);
	else if (currentWidget == "add-bookmarks")
		addAction(m_actions->addBookmarks);
	else if (currentWidget == "bookmarks-manager")
		addAction(m_actions->bookmarksManager);
	else if (currentWidget == "new-tab")
		addAction(m_actions->newTab);
	else if (currentWidget == "new-window")
		addAction(m_actions->newWindow);
	else {
		QMessageBox::warning(this, tr("Probleme"), tr("Une erreur est présente dans le thème."
			"La barre de navigation risque de ne pas se charger comme prévu"
			"Nous vous conseillons de contacter le créateur du thème pour qu'il corrige l'erreur au plus vite."));
	}
}
