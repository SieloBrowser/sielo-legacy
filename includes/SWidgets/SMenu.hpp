#pragma once

#include <QMenu>
#include <QStandardItem>

class SMainWindow;
class SBookmarksView;
class SActions;

enum SMenuType {
	File = 0,
	Show = 1,
	Brows = 2,
    Dl = 3,
    Fav = 4,
    Edit = 5,
    About = 6
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
private:
    void createBookmarksItem(QStandardItem *item, SMenu *parent);

	SMainWindow* m_parent{ nullptr };
    QSharedPointer<SActions> m_actions{};

    SBookmarksView *m_bView{ nullptr };
};
