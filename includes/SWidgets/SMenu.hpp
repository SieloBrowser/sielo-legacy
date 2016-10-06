#pragma once

#include <QMenu>

class SMainWindow;
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
private:
	SMainWindow* m_parent{ nullptr };
    QSharedPointer<SActions> m_actions{};
};
