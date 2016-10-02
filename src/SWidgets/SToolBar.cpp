#include "includes/SWidgets/SToolBar.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

SToolBar::SToolBar(const QString& toolBarName, SMainWindow* parent) :
	QToolBar(toolBarName),
	m_parent(parent),
	m_actions(parent->getActions())
{
	m_spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
/*
	std::ofstream out{ "Themes/SIcons/toolBar.txt" };

	if (out.fail())
		throw std::runtime_error("failed to whrite default theme");

	out << THEME_V0 << ' ';
	out << 2 << ' ';
	out << 4 << ' ';
	out << "back" << ' ';
	out << "next" << ' ';
	out << "home" << ' ';
	out << "refresh" << ' ';
	out << 2 << ' ';
	out << "go" << ' ';
	out << "search" << ' ';
*/
		
}


SToolBar::~SToolBar()
{
}

void SToolBar::loadToolBarV0(QTextStream & in)
{
	unsigned widgetInToolBar{ 0 };
	QString currentWidget{ "nothing" };

	addWidget(m_spacer1);

	in >> widgetInToolBar;
    for (size_t i{ 0 }; i < widgetInToolBar; ++i) {
		in >> currentWidget;
		if (currentWidget == "back") {
			addAction(m_actions->back);
		}
		else if (currentWidget == "next") {
			addAction(m_actions->next);
		}
		else if (currentWidget == "home") {
			addAction(m_actions->home);
		}
		else if (currentWidget == "refresh") {
			addAction(m_actions->refreshOrStop);
		}
		else if (currentWidget == "go") {
			addAction(m_actions->go);
		}
		else if (currentWidget == "search") {
			addAction(m_actions->shearch);
		}
		else if (currentWidget == "history") {
			addAction(m_actions->showHistory);
		}
		else if (currentWidget == "preferences") {
			addAction(m_actions->preferences);
		}
		else if (currentWidget == "addBookmarks") {
			addAction(m_actions->addBookmarks);
		}
		else if (currentWidget == "bookmarksManager") {
			addAction(m_actions->bookmarksManager);
		}
		else if (currentWidget == "urlArea") {
			addWidget(m_parent->getUrlArea());
		}
		else if (currentWidget == "searchArea") {
			addWidget(m_parent->getSearchArea());
		}
		else if (currentWidget == "separator") {
			addSeparator();
		}
		else {
			QMessageBox::warning(this, "Problème", "Une erreur est présente à la ligne numéro " + QString::number(i + 1) + ". "
				"La barre de navigation risque de ne pas se charger comme prévue"
				"Nous vous conseillons de contacter le créateur du thème pour qu'il corrige l'erreur au plus vite.");
		}
	}

	addWidget(m_spacer2);

}
