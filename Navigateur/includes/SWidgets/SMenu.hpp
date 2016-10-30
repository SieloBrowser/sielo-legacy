#pragma once

#include <QMenu>
#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QDialogButtonBox>
#include <QStandardItem>
#include <QNetworkReply>

class SMainWindow;
class SBookmarksView;
class SActions;

class AboutSN : public QDialog
{
public:
	// Constructor and destrcutor
	AboutSN(QWidget *parent = nullptr);
	~AboutSN();
	
private:
	// Layouts
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };
	QGridLayout *m_txtLayout{ new QGridLayout() };

	// Widgets in the window
	QLabel *m_SNIcon{ new QLabel(this) };
	QSpacerItem *m_spacer{ new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Maximum) };
	QLabel *m_txt{ new QLabel(this) };
	QDialogButtonBox *m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this) };

};
enum SMenuType {
	File = 0,
	Show = 1,
    Brows = 2,
    Dl = 3,
    Theme = 4,
    Fav = 5,
    Edit = 6,
    About = 7
};

class SMenu : public QMenu
{
public:
	SMenu(SMainWindow* parent, const QString& name, SMenuType menuType);
	~SMenu();

	void createFileMenu();
	void createShowMenu();
    void createBrowsMenu();
    void createDlMenu();
    void createBookmarksMenu();
    void createThemeMenu();
    void createEditMenu();
	void createAboutMenu();

	void reset(); 

public slots:
	void createNewWindows();
	void openFile();

	void findInPage();
	void zoom();

    void showHistory();
    void privateBrowsing();
    void showPageSrc();

    void openBookmarksManager();
    void openBookmark();
    void addBookmark();

    void changeTheme(QAction *theme);
    void addTheme();

    void openPreferencesDialog();

	void openAboutSielo();
	void checkMaJ();
private:
    void createBookmarksItem(QStandardItem *item, SMenu *parent);

    SMainWindow* m_parent{ nullptr };
    QSharedPointer<SActions> m_actions{};

    SBookmarksView *m_bView{ nullptr };
    QMenu *m_themesMenu{ nullptr };
};
