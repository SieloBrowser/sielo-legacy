#pragma once

#include <QAction>
#include "includes/SMainWindow.hpp"

struct SActions {

    SActions() {}

    int currentTheme{ SMainWindow::SSettings->value("preferences/themes/currentTheme", 0).toInt() };
    QString themePath{ SMainWindow::dataPath + "Themes/" + SMainWindow::SSettings->value("preferences/themes/" + QString::number(currentTheme) + "/name", "SIcons").toString() + "/" };

    // All actions possible in the tool bar
    QAction* back{ new QAction(QIcon(themePath + "back.png"), QObject::trUtf8("&Précédent")) };
    QAction* next{ new QAction(QIcon(themePath + "next.png"), "&Suivant") };
    QAction* home{ new QAction(QIcon(themePath + "home.png"), "&Accueil") };
    QAction* refreshOrStop{ new QAction(QIcon(themePath + "refresh.png"), "&Rafraichir") };
    QAction* go{ new QAction(QIcon(themePath + "go.png"), "&Go") };
    QAction* shearch{ new QAction(QIcon(themePath + "search.png"), "&Chercher") };
    QAction* showHistory{ new QAction(QIcon(themePath + "history.png"), "&Historique") };
    QAction* preferences{ new QAction(QIcon(themePath + "preferences.png"), "Pré&férences") };
    QAction* addBookmarks{ new QAction(QIcon(themePath + "addFavoris.png"), "&Nouveau Favoris") };
    QAction* bookmarksManager{ new QAction(QIcon(themePath + "favoris.png"), "&Géré les favoris") };

    // All other actions
    QAction* newTab{ new QAction(QIcon(themePath + "newTab.png"), "Nouvelle Onglet") };
    QAction* newWindow{ new QAction(QIcon(themePath + "newWindow.png"), "Nouvelle Fenêtre") };
    QAction* openFile{ new QAction("Ouvrir un fichier") };
    QAction* exit{ new QAction(QIcon(themePath + "exit.png"), "Fermer Sielo NAvigateur") };
    QAction* showFullScreen{ new QAction("Afficher en pleine écran") };
    QAction* findInPage{ new QAction(QIcon(themePath + "search.png"), "Chercher dans la page") };
    QAction* findNext{ new QAction("Trouver le suivant") };
    QAction* findPrevious{ new QAction("Trouver le précédent") };
    QAction* zoomMore{ new QAction("Zoomer plus") };
    QAction* zoomLess{ new QAction("Zoomer moins") };
    QAction* history{ new QAction(QIcon(themePath + "history.png"), "Historique") };
    QAction* privateBrowsing{ new QAction("Nouvelle fenêtre navigation privée") };
    QAction* downloads{ new QAction("Téléchargements") };
    QAction* viewPageCodeSource{ new QAction("Voire le code source de la page") };
    QAction* aboutQt{ new QAction("A propos de Qt") };
    QAction* aboutSielo{ new QAction("A propos de Sielo NAvigateur") };

};
