#pragma once

#include <QMenu>

class SMainWindow;
class Actions;

enum SMenuType {
	File = 0,
	Show = 1,
	Brows = 2,
	Fav = 3,
	Edit = 4,
	About = 5
};

class SMenu : public QMenu
{
public:
	SMenu(SMainWindow* parent, const QString& name, SMenuType menuType);
	~SMenu();

	void createFileMenu();
	void createShowMenu();
	void createBrowsMenu();
	void createEditMenu();
	void createAboutMenu();

	void reset(); 

public slots:
	void createNewWindows();
	void openFile();

	void findInPage();
	void zoom();
private:
	SMainWindow* m_parent{ nullptr };
	Actions* m_actions{ nullptr };
};