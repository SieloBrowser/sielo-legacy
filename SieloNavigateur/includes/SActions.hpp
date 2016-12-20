#pragma once

#include <QAction>
#include "includes/SMainWindow.hpp"

struct SActions {

	SActions() {}

	int currentTheme{ SMainWindow::SSettings->value("preferences/themes/currentTheme", 0).toInt() };
	QString themePath{ SMainWindow::dataPath + "Themes/" + SMainWindow::SSettings->value("preferences/themes/" + QString::number(currentTheme) + "/name", "SIcons").toString() + "/" };

	// All actions possible in the tool bar
	QAction* back{ new QAction(QIcon(themePath + "back.png"), QObject::tr("&Précédent")) };
	QAction* next{ new QAction(QIcon(themePath + "next.png"), QObject::tr("&Suivant")) };
	QAction* home{ new QAction(QIcon(themePath + "home.png"), QObject::tr("&Accueil")) };
	QAction* refreshOrStop{ new QAction(QIcon(themePath + "refresh.png"), QObject::tr("&Rafraichir")) };
	QAction* go{ new QAction(QIcon(themePath + "go.png"), QObject::tr("&Go")) };
	QAction* shearch{ new QAction(QIcon(themePath + "search.png"), QObject::tr("&Chercher")) };
	QAction* history{ new QAction(QIcon(themePath + "history.png"), QObject::tr("Historique")) };
	QAction* preferences{ new QAction(QIcon(themePath + "preferences.png"), QObject::tr("Pré&férences")) };
    QAction* addBookmarks{ new QAction(QIcon(themePath + "addBookmarks.png"), QObject::tr("&Nouveau favori")) };
    QAction* bookmarksManager{ new QAction(QIcon(themePath + "bookmarksManager.png"), QObject::tr("&Gérer les favoris")) };

	// All other actions
	QAction* newTab{ new QAction(QIcon(themePath + "newTab.png"), QObject::tr("Nouvel onglet")) };
	QAction* newWindow{ new QAction(QIcon(themePath + "newWindow.png"), QObject::tr("Nouvelle fenêtre")) };
	QAction* openFile{ new QAction(QObject::tr("Ouvrir un fichier")) };
	QAction* exit{ new QAction(QIcon(themePath + "exit.png"), QObject::tr("Fermer Sielo Navigateur")) };
	QAction* showFullScreen{ new QAction(QObject::tr("Afficher en plein écran")) };
	QAction* showMenuBar{ new QAction(QObject::tr("Cacher la barre de menu")) };
    QAction* split{ new QAction(QObject::tr("Séparer les onglets en deux")) };
	QAction* findInPage{ new QAction(QIcon(themePath + "search.png"), QObject::tr("Chercher dans la page")) };
	QAction* findNext{ new QAction(QObject::tr("Trouver le suivant")) };
	QAction* findPrevious{ new QAction(QObject::tr("Trouver le précédent")) };
	QAction* zoomMore{ new QAction(QObject::tr("Zoomer plus")) };
	QAction* zoomLess{ new QAction(QObject::tr("Zoomer moins")) };
	QAction* privateBrowsing{ new QAction(QObject::tr("Nouvelle fenêtre de navigation privée")) };
	QAction* viewPageCodeSource{ new QAction(QObject::tr("Voir le code source de la page")) };
    QAction* addTheme{ new QAction(QObject::tr("Ajouter un thème")) };
    QAction* openThemeEditor{ new QAction(QObject::tr("Editeur de thème")) };
    QAction* aboutQt{ new QAction(QObject::tr("A propos de Qt")) };
	QAction* aboutSielo{ new QAction(QObject::tr("A propos de Sielo Navigateur")) };
	QAction* checkMaJ{ new QAction(QObject::tr("Vérifier les mises à jour")) };
};
