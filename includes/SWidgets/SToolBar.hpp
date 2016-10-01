#pragma once

#include <QToolBar>
#include <QString>
#include <QTextStream>

class SMainWindow;
class SActions;

class SToolBar : public QToolBar
{
public:
	SToolBar(const QString& toolBarName, SMainWindow* parent);
	~SToolBar();

	void loadToolBarV0(QTextStream& in);
private:
	SMainWindow* m_parent{ nullptr };
	QWidget* m_spacer1{ new QWidget(this) };
	QWidget* m_spacer2{ new QWidget(this) };
    QSharedPointer<SActions> m_actions{};
};

