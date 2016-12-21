#pragma once

#include <QToolBar>
#include <QString>
#include <QTextStream>

class SMainWindow;
struct SActions;

class SToolBar : public QToolBar
{
public:
	// Constructor and destructor
	SToolBar(const QString& toolBarName, SMainWindow* parent);
	~SToolBar();

	// Function to load toolbar in version 0 (for keeping older versions) 
	void loadToolBarV0(QTextStream& in);
	void loadToolBarV1(QTextStream& in);
private:
	SMainWindow* m_parent{ nullptr };

	QWidget* m_spacer1{ new QWidget(this) };
	QWidget* m_spacer2{ new QWidget(this) };

    QSharedPointer<SActions> m_actions{};
};

