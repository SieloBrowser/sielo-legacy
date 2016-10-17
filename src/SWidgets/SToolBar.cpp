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
			QMessageBox::warning(this, "Probleme", "Une erreur est présente a la ligne numéro " + QString::number(i + 1) + ". "
				"La barre de navigation risque de ne pas se charger comme prévue"
				"Nous vous conseillons de contacter le créateur du theme pour qu'il corrige l'erreur au plus vite.");
		}
	}

	addWidget(m_spacer2);

}
