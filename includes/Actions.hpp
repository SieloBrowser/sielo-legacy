#pragma once

#include <QAction>

struct Actions {

	QAction* back{ new QAction(QIcon("Themes/Sicons/back.png"), "&Précédent") };
	QAction* next{ new QAction(QIcon("Themes/Sicons/next.png"), "&Suivant") };;
	QAction* home{ new QAction(QIcon("Themes/Sicons/home.png"), "&Accueil") };;
	QAction* refreshOrStop{ new QAction(QIcon("Themes/Sicons/refresh.png"), "&Rafraichir") };;
	QAction* go{ new QAction(QIcon("Themes/Sicons/go.png"), "&Go") };;
	QAction* shearch{ new QAction(QIcon("Themes/Sicons/search.png"), "&Chercher") };
	QAction* showHistory{ new QAction(QIcon("Themes/Sicons/history.png"), "&Historique") };
	QAction* preferences{ new QAction(QIcon("Themes/Sicons/preferences.png"), "Pré&férences") };
	QAction* addBookmarks{ new QAction(QIcon("Themes/Sicons/add-favories.png"), "&Nouveau Favoris") };
	QAction* bookmarksManager{ new QAction(QIcon("Themes/Sicons/favories.png"), "&Géré les favoris") };
};