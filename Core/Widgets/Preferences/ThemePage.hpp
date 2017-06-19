/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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
#ifndef SIELO_BROWSER_THEMEPAGE_HPP
#define SIELO_BROWSER_THEMEPAGE_HPP

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QListWidget>
#include <QLabel>
#include <QPushButton>

#include <QHash>

namespace Sn {
class ThemePage: public QWidget {
Q_OBJECT

public:
	ThemePage(QWidget* parent);
	~ThemePage();

	void save();

private slots:
	void loadSettings();

	void currentChanged();
	void showLicense();

	void addTheme();
private:
	struct Theme {
		bool isValid{};
		QIcon icon{};
		QString name{};
		QString author{};
		QString shortDesc{};
		QString longDesc{};
		QString license{};
	};

	Theme parseTheme(const QString& path, const QString& name);

	void setupUI();

	QString m_activeTheme{};
	QHash<QString, Theme> m_themeHash;

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_nameLayout{nullptr};
	QFormLayout* m_areaLayout{nullptr};

	QListWidget* m_themeList{nullptr};
	QWidget* m_areaWidget{nullptr};

	QLabel* m_nameLabel{nullptr};
	QLabel* m_name{nullptr};
	QPushButton* m_licenseBtn{nullptr};
	QLabel* m_authorLabel{nullptr};
	QLabel* m_author{nullptr};
	QLabel* m_descLabel{nullptr};
	QLabel* m_desc{nullptr};
	QPushButton* m_addThemeButton{nullptr};

};
}

#endif //SIELO_BROWSER_THEMEPAGE_HPP
