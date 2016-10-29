#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>
#include <QHash>
#include <QVector>
#include <QCloseEvent>

#include "includes/Widgets/ToolBar.hpp"

class HelpDialog : public QDialog
{
public:
	HelpDialog(QWidget *parent = nullptr);
	~HelpDialog();

private:
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QLabel *m_label{ new QLabel(this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this) };
};
class MainWindow : public QMainWindow
{
public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	void createMenus();
	void createActions();
    void loadToolBar(const QString &filePath);

	void newThm();
	void openThm();
	void saveThm();
	void saveThmAs();

	void closeThm();

	void changeIcon(QAction *action);

	ToolBar* addNewToolBar(Qt::ToolBarArea area = Qt::TopToolBarArea);
	void createNewToolBar();
	void deleteToolBar(ToolBar *toolBar);

	QAction *m_backAction{ new QAction(QObject::tr("&Précédent"), this) };
	QAction *m_nextAction{ new QAction(QObject::tr("&Suivant"), this) };
	QAction *m_homeAction{ new QAction(QObject::tr("&Accueil"), this) };
	QAction *m_refreshOrStopAction{ new QAction(QObject::tr("&Rafraichir"), this) };
	QAction *m_goAction{ new QAction(QObject::tr("&Go"), this) };
	QAction *m_searchAction{ new QAction(QObject::tr("Chercher"), this) };
	QAction *m_sowHistory{ new QAction(QObject::tr("&Historique"), this) };
	QAction *m_preferencesAction{ new QAction(QObject::tr("Pré&férences"), this) };
	QAction *m_addBookmarksAction{ new QAction(QObject::tr("&Nouveau favori"), this) };
	QAction *m_bookmarsManagerAction{ new QAction(QObject::tr("&Gérer les favoris")) };
	QAction *m_newTabAction{ new QAction(QObject::tr("Nouvel onglet"), this) };
	QAction *m_newWindowAction{ new QAction(QObject::tr("Nouvelle fenêtre"), this) };
	QAction *m_exitAction{ new QAction(QObject::tr("Fermer le navigateur"), this) };

	QVector<QLineEdit*> m_urlAreas{};
	QVector<QLineEdit*> m_searchAreas{};
	QVector<QWidget*> m_spacers{};

	QHash<QString, QAction*> m_editableAction{};

	bool thmSaved{ true };

public slots:
	void unsaveThm();
	void openHelp();

protected:
	void closeEvent(QCloseEvent *event);

private:
	void copyDir(QString src, QString dst);

	QString m_savePath{};
	QString m_thmPath{};
	QString m_thmName{};
	QString m_savedThmPath{};

	// Actions for menus
	QAction *m_newThm{ new QAction(QObject::tr("Nouveau thème"), this) };
	QAction *m_openThm{ new QAction(QObject::tr("Ouvrir un thème"), this) };
	QAction *m_saveThm{ new QAction(QObject::tr("Sauvegarder le thème"), this) };
	QAction *m_saveThmAs{ new QAction(QObject::tr("Sauvegarder le thème sous..."), this) };

	QAction *m_newToolBar{ new QAction(QObject::tr("Nouvelle barre d'outils"), this) };
	QAction *m_helpAction{ new QAction(QObject::tr("Aide") , this) };

	QMenu *m_iconsMenu{ new QMenu(QObject::tr("&Icones"), this) };

    QWebEngineView *m_view{ new QWebEngineView(this) };
	QVector<ToolBar*> m_toolBars{};
};
