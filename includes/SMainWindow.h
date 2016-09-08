#pragma once

#include <QMainWindow>
#include <QWidget>

#include "STabWidget.h"
#include "SWebView.h"

class SMainWindow : public QMainWindow
{
public:
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

private:
	STabWidget* m_tabs{ new STabWidget(this) };
};

