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
	QWidget* m_spacer1{ new QWidget(this) };
	QWidget* m_spacer2{ new QWidget(this) };
	Actions* m_actions{ nullptr };
};

