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

#pragma once
#ifndef SIELO_BROWSER_MODELMENU_HPP
#define SIELO_BROWSER_MODELMENU_HPP

#include "SharedDefines.hpp"

#include <QMenu>
#include <QAction>

#include <QAbstractItemModel>

namespace Sn {
class SIELO_SHAREDLIB ModelMenu: public QMenu {
Q_OBJECT

public:
	ModelMenu(QWidget* parent = nullptr);

	QAbstractItemModel* model() const { return m_model; }
	void setModel(QAbstractItemModel* model);

	int maxRows() const { return m_maxRows; }
	void setMaxRows(int max);

	int firstSeparator() const { return m_firstSeparator; }
	void setFirstSeparator(int offset);

	QModelIndex rootIndex() const { return m_root; }
	void setRootIndex(const QModelIndex& index);

	int hoverRole() const { return m_hoverRole; }
	void setHoverRole(int role);

	int separatorRole() const { return m_separatorRole; }
	void setSeparatorRole(int role);

	QAction* createAction(const QIcon& icon, const QString& text, QObject* parent);

signals:
	void activated(const QModelIndex& index);
	void hovered(const QString& text);

protected:
	virtual bool prePopulated();
	virtual void postPopulated();

	void createMenu(const QModelIndex& parent, int max, QMenu* parentMenu = nullptr, QMenu* menu = nullptr);

private slots:
	void aboutToShow();
	void triggered(QAction* action);
	void hovered(QAction* action);

private:
	QAction* createAction(const QModelIndex& index);

	int m_maxRows{7};
	int m_firstSeparator{-1};
	int m_maxWidth{-1};
	int m_hoverRole{0};
	int m_separatorRole{0};

	QAbstractItemModel* m_model{nullptr};
	QPersistentModelIndex m_root{};
};

}

#endif //SIELO_BROWSER_MODELMENU_HPP
