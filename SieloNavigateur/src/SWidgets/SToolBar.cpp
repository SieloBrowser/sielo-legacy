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
	QMessageBox::information(m_parent, "DEBUG", "Color is: " + winFrameColor.name());
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
