#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QSettings>
#include <QLineEdit>

#include "STabWidget.h"
#include "SWebView.h"
#include "SToolBar.h"
#include "Actions.h"

class SMainWindow : public QMainWindow
{
public:
	static QSettings* SSettings;
	
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

	bool loadToolBar(const QString& filePath);

	// Getters
	Actions* getActions() const { return m_actions; }
	QLineEdit* getUrlArea() const { return m_urlArea; }
	QLineEdit* getSearchArea() const { return m_searchArea; }

	STabWidget* m_tabs{ new STabWidget(this) };
private:
	// Private member
	QLineEdit* m_urlArea{ new QLineEdit(this) };
	QLineEdit* m_searchArea{ new QLineEdit(this) };
	Actions* m_actions{ new Actions };
	QVector<SToolBar*> m_toolsBars{};
};

