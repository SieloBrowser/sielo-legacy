#pragma once

#include <QLineEdit>
#include <QPainter>
#include <QIcon>

class SMainWindow;

class SSearchArea : public QLineEdit
{
public:
	SSearchArea(const QIcon& icon, SMainWindow* parent = nullptr);

	void paintEvent(QPaintEvent* event);

private:
	SMainWindow* m_parent{ nullptr };
	QIcon m_icon{};
};