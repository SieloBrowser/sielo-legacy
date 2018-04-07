/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "ModelMenu.hpp"

#include <QFontMetrics>

Q_DECLARE_METATYPE(QModelIndex)
namespace Sn {

ModelMenu::ModelMenu(QWidget* parent) :
	QMenu(parent),
	m_maxRows(7),
	m_firstSeparator(-1),
	m_maxWidth(-1),
	m_hoverRole(0),
	m_separatorRole(0)
{
	connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
}

void ModelMenu::setModel(QAbstractItemModel* model)
{
	m_model = model;
}

void ModelMenu::setMaxRows(int max)
{
	m_maxRows = max;
}

void ModelMenu::setFirstSeparator(int offset)
{
	m_firstSeparator = offset;
}

void ModelMenu::setRootIndex(const QModelIndex& index)
{
	m_root = index;
}

void ModelMenu::setHoverRole(int role)
{
	m_hoverRole = role;
}

void ModelMenu::setSeparatorRole(int role)
{
	m_separatorRole = role;
}

QAction* ModelMenu::createAction(const QIcon& icon, const QString& text, QObject* parent)
{
	QFontMetrics fontMetrics{font()};

	if (m_maxWidth == -1)
		m_maxWidth = fontMetrics.width(QLatin1Char('m')) * 30;

	QString smallText{fontMetrics.elidedText(text, Qt::ElideMiddle, m_maxWidth)};

	return new QAction(icon, smallText, parent);
}

bool ModelMenu::prePopulated()
{
	return false;
}

void ModelMenu::postPopulated()
{

}

void ModelMenu::createMenu(const QModelIndex& parent, int max, QMenu* parentMenu, QMenu* menu)
{
	if (!menu) {
		QString title{parent.data().toString()};
		QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));

		menu = new QMenu(title, this);
		menu->setIcon(icon);

		parentMenu->addMenu(menu);

		QVariant variant{};
		variant.setValue(parent);

		menu->menuAction()->setData(variant);

		connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));

		return;
	}

	int end{m_model->rowCount(parent)};

	if (max != -1)
		end = qMin(max, end);

	disconnect(menu, SIGNAL(triggered(QAction * )), this, SLOT(triggered(QAction * )));
	disconnect(menu, SIGNAL(hovered(QAction * )), this, SLOT(hovered(QAction * )));
	connect(menu, SIGNAL(triggered(QAction * )), this, SLOT(triggered(QAction * )));
	connect(menu, SIGNAL(hovered(QAction * )), this, SLOT(hovered(QAction * )));

	for (int i{0}; i < end; ++i) {
		QModelIndex index{m_model->index(i, 0, parent)};

		if (m_model->hasChildren(index))
			createMenu(index, -1, menu);
		else {
			if (m_separatorRole != 0 && index.data(m_separatorRole).toBool())
				addSeparator();
			else
				menu->addAction(createAction(index));
		}

		if (menu == this && i == m_firstSeparator - 1)
			addSeparator();
	}
}

void ModelMenu::aboutToShow()
{
	if (QMenu* menu = qobject_cast<QMenu*>(sender())) {
		QVariant variant{menu->menuAction()->data()};

		if (variant.canConvert<QModelIndex>()) {
			QModelIndex index = qvariant_cast<QModelIndex>(variant);

			createMenu(index, -1, menu, menu);
			disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
			disconnect(menu, SIGNAL(triggered(QAction * )), this, SLOT(triggered(QAction * )));
			disconnect(menu, SIGNAL(hovered(QAction * )), this, SLOT(hovered(QAction * )));

			return;
		}
	}

	clear();

	if (prePopulated())
		addSeparator();

	int max{m_maxRows};

	if (max != -1)
		max += m_firstSeparator;

	createMenu(m_root, max, this, this);
	postPopulated();
}

void ModelMenu::triggered(QAction* action)
{
	QVariant variant{action->data()};

	if (variant.canConvert<QModelIndex>()) {
		QModelIndex index = qvariant_cast<QModelIndex>(variant);
		emit activated(index);
	}
}

void ModelMenu::hovered(QAction* action)
{
	QVariant variant{action->data()};

	if (variant.canConvert<QModelIndex>()) {
		QModelIndex index = qvariant_cast<QModelIndex>(variant);
		QString hoveredString{index.data(m_hoverRole).toString()};

		if (!hoveredString.isEmpty())
			emit hovered(hoveredString);
	}
}

QAction* ModelMenu::createAction(const QModelIndex& index)
{
	QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
	QAction* action = createAction(icon, index.data().toString(), this);
	QVariant variant{};

	variant.setValue(index);
	action->setData(variant);

	return action;
}

}