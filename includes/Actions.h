#pragma once

#include <QAction>

struct Actions {
	QAction* back{ new QAction(QIcon("Sicons/back.png"), "&Précédent") };
	QAction* next{ new QAction(QIcon("Sicons/next.png"), "&Suivant") };;
	QAction* home{ new QAction(QIcon("Sicons/home.png"), "&Accueil") };;
	QAction* refreshOrStop{ new QAction(QIcon("Sicons/refresh.png"), "&Rafraichir") };;
	QAction* go{ new QAction(QIcon("Sicons/go.png"), "&Go") };;
};