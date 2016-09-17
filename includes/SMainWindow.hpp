#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QSettings>
#include <QLineEdit>

#include "..\includes\STabWidget.hpp"
#include "..\includes\SWebView.hpp"
#include "..\includes\SToolBar.hpp"
#include "..\includes\Actions.hpp"

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

public slots:
	void changeTitle(const QString& newTitle);
	void changeUrl(const QUrl& newUrl);

private:
	// Private member
	QLineEdit* m_urlArea{ new QLineEdit(this) };
	QLineEdit* m_searchArea{ new QLineEdit(this) };
	Actions* m_actions{ new Actions };
	STabWidget* m_tabs{ new STabWidget(this) };
	QVector<SToolBar*> m_toolsBars{};
};

