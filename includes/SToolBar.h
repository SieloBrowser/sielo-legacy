#pragma once
#include <QToolBar>
#include <QString>

#include "Actions.h"

class SMainWindow;

class SToolBar : public QToolBar
{
public:
	SToolBar(const QString& toolBarName, SMainWindow* parent);
	~SToolBar();

private:
	SMainWindow* m_parents{ nullptr };
	Actions* m_actions{ nullptr };
};

