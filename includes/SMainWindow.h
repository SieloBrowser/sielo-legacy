#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QSettings>

#include "STabWidget.h"
#include "SWebView.h"
#include "SToolBar.h"
#include "Actions.h"

class SMainWindow : public QMainWindow
{
public:
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

	Actions* getActions() const { return m_actions; }

	static QSettings* SSettings;

private:
	Actions* m_actions{ new Actions };
	STabWidget* m_tabs{ new STabWidget(this) };
	SToolBar* m_toolBar{ new SToolBar("Teste", this) };
};

