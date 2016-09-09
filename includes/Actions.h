#pragma once

#include <QAction>

struct Actions {
	QAction* back{ new QAction(QIcon("SSicons/back.png"), "&Précédent") };
	QAction* next{ new QAction(QIcon("SSicons/next.png"), "&Suivant") };;
	QAction* home{ new QAction(QIcon("SSicons/home.png"), "&Accueil") };;
	QAction* refreshOrStop{ new QAction(QIcon("SSicons/refresh.png"), "&Rafraichir") };;
	QAction* go{ new QAction(QIcon("SSicons/go.png"), "&Go") };;
};