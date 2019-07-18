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
#ifndef SIELO_BROWSER_HTML5PERMISSIONSDIALOG_HPP
#define SIELO_BROWSER_HTML5PERMISSIONSDIALOG_HPP

#include "SharedDefines.hpp"

#include <QDialog>
#include <QWidget>

#include <QHash>
#include <QStringList>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QTreeWidget>
#include <QSpacerItem>

#include <QWebEngine/WebPage.hpp>

namespace Sn {
class SIELO_SHAREDLIB HTML5PermissionsDialog: public QDialog {
Q_OBJECT

public:
	HTML5PermissionsDialog(QWidget* parent = nullptr);
	~HTML5PermissionsDialog();

	void showFeaturePermissions(Engine::WebPage::Feature feature);

private slots:
	void removeEntry();
	void featureIndexChanged();

	void saveSettings();

private:
	enum Role { Allow, Deny };

	void setupUI();
	void loadSettings();

	Engine::WebPage::Feature currentFeature() const;

	QGridLayout* m_layout{nullptr};
	QVBoxLayout* m_removeLayout{nullptr};
	QSpacerItem* m_removeSpacer{nullptr};
	QPushButton* m_remove{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};
	QTreeWidget* m_permissionsTree{nullptr};
	QHBoxLayout* m_featureLayout{nullptr};
	QLabel* m_descFeature{nullptr};
	QComboBox* m_feature{nullptr};
	QSpacerItem* horizontalSpacer{nullptr};

	QHash<Engine::WebPage::Feature, QStringList> m_granted{};
	QHash<Engine::WebPage::Feature, QStringList> m_denied{};
};

}

#endif //SIELO_BROWSER_HTML5PERMISSIONSDIALOG_HPP
