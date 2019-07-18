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
#ifndef SIELO_BROWSER_APPEARANCE_HPP
#define SIELO_BROWSER_APPEARANCE_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QGroupBox>

#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include <QHash>

namespace Sn
{
class PreferencesDialog;

class SIELO_SHAREDLIB AppearancePage: public QWidget {
Q_OBJECT

public:
	AppearancePage(QWidget* parent, PreferencesDialog* perferencesDialog);
	~AppearancePage();

	void save();

	static QString colorString(QString id);
private slots:
	void loadSettings();
	void currentChanged();

	void showLicense();

	void tabsSpacesPaddingValueChanged(int value);
	void blurFilterRadiusValueChanged(int value);
	void backgroundLocationClicked();
	void openGallery();
	void addTheme();
	void getColor();
	void showFloatingButtonChanged(bool enabled);

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

	QTabWidget* m_tabs{nullptr};
	QWidget* m_themePage{nullptr};
	QWidget* m_advancedPage{nullptr};

	QGroupBox* m_themeBox{nullptr};
	QGroupBox* m_themeColorsBox{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QVBoxLayout* m_themePageLayout{nullptr};
	QVBoxLayout* m_advancedPageLayout{nullptr};
	QVBoxLayout* m_themeLayout{nullptr};
	QGridLayout* m_themeColorsLayout{nullptr};
	QHBoxLayout* m_nameLayout{nullptr};
	QFormLayout* m_areaLayout{nullptr};
	QHBoxLayout* m_themeActionLayout{nullptr};
	QHBoxLayout* m_backgroundLayout{nullptr};
	QVBoxLayout* m_blurFilterLayout{nullptr};

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
	QPushButton* m_viewGalleryButton{nullptr};

	QPushButton* m_mainColorButton{nullptr};
	QPushButton* m_secondColorButton{nullptr};
	QPushButton* m_accentColorButton{nullptr};
	QPushButton* m_textColorButton{nullptr};

	QCheckBox* m_fullyLoadThemes{nullptr};

	QCheckBox* m_showFloatingButton{nullptr};
	QCheckBox* m_floatingButtonFoloweMouse{nullptr};
	QCheckBox* m_hideControles{nullptr};
	QCheckBox* m_hideBookmarksHistoryActionsByDefault{nullptr};
	QCheckBox* m_bottomNavigationBar{nullptr};

	QLabel* m_tabsSpacesPaddingLabel{nullptr};
	QSlider* m_tabsSpacesPadding{nullptr};
	QLabel* m_blurFilterRadiusLabel{nullptr};
	QSlider* m_blurFilterRadius{nullptr};
	QCheckBox* m_repeatBackground{nullptr};
	QLabel* m_backgroundLabel{nullptr};
	QLineEdit* m_backgroundLocationEdit{nullptr};
	QPushButton* m_backgroundLocationButton{nullptr};
#ifdef Q_OS_WIN
	QLabel* m_oldChipsetInfoLabel{nullptr};
#endif
	QSpacerItem* m_spacer{nullptr};

	bool m_colorsChanged{false};
	PreferencesDialog* m_preferencesDialog{nullptr};
};
}

#endif //SIELO_BROWSER_APPEARANCE_HPP
