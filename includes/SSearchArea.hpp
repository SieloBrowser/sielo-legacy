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

public slots:
	void loadSearch();

private:
	SMainWindow* m_parent{ nullptr };
	QIcon m_icon{};
};