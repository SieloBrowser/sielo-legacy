#pragma once

#include <QLineEdit>
#include <QPainter>
#include <QIcon>

class SMainWindow;

class SSearchArea : public QLineEdit
{
public:
	// Constructor and destuctor
	SSearchArea(const QIcon& icon, SMainWindow* parent = nullptr);
	~SSearchArea();

	// Use for paint search icon befor line edit
	void paintEvent(QPaintEvent* event);

public slots:
	void loadSearch(); //< slot to do the search on the web

private:
	SMainWindow* m_parent{ nullptr };
	QIcon m_icon{};
};