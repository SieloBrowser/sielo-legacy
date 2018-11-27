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
#ifndef SIELOBROWSER_HISTORYMANAGER_HPP
#define SIELOBROWSER_HISTORYMANAGER_HPP

#include "SharedDefines.hpp"

#include <QDialog>

#include <QKeyEvent>

#include <QGridLayout>

#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>

#include <QPointer>

namespace Sn
{
class HistoryTreeView;

class BrowserWindow;

class SIELO_SHAREDLIB HistoryManager: public QDialog {
Q_OBJECT

public:
	HistoryManager(BrowserWindow* window, QWidget* parent = nullptr);
	~HistoryManager() = default;

	void setMainWindow(BrowserWindow* window);

public slots:
	void search(const QString &searchText);

private slots:
	void urlActivated(const QUrl &url);
	void urlCtrlActivated(const QUrl &url);
	void urlShiftActivated(const QUrl &url);

	void openUrl(const QUrl &url = QUrl());
	void openUrlInNewTab(const QUrl &url = QUrl());
	void openUrlInNewWindow(const QUrl &url = QUrl());
	void openUrlInNewPrivateWindow(const QUrl &url = QUrl());

	void createContextMenu(const QPoint &pos);

	void copyUrl();
	void copyTitle();
	void clearHistory();

private:
	void keyPressEvent(QKeyEvent* event) override;
	void setupUI();

	BrowserWindow* getWindow();

	QPointer<BrowserWindow> m_window{};

	QGridLayout* m_layout{nullptr};

	QSpacerItem* m_searchSpacer{ nullptr };
	QLineEdit* m_search{ nullptr };
	HistoryTreeView* m_view{nullptr};
	QPushButton* m_clearButton{nullptr};
	QPushButton* m_deleteAllButton{nullptr};
	QSpacerItem* m_spacer{nullptr};
};
}

#endif //SIELOBROWSER_HISTORYMANAGER_HPP
