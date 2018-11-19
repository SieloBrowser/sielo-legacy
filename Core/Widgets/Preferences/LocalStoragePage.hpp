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
#ifndef SIELO_BROWSER_LOCALSTORAGEPAGE_HPP
#define SIELO_BROWSER_LOCALSTORAGEPAGE_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>

namespace Sn {

class SIELO_SHAREDLIB LocalStoragePage: public QWidget {
Q_OBJECT

public:
	LocalStoragePage(QWidget* parent);
	~LocalStoragePage();

	void loadSettings();
	void save();

private slots:
	void allowNetworkStorageChanged(bool allow);
	void allowSavingHistoryChanged(bool allow);
	void allowHTML5StorageChanged(bool allow);

	void cacheValueChanged(int value);

	void changeStorageLocationClicked();
	void deleteHTML5StorageClicked();

private:
	void setupUI();

	QVBoxLayout* m_layout{nullptr};

	QGridLayout* m_networkStorageLayout{nullptr};
	QHBoxLayout* m_MBLayout{nullptr};
	QHBoxLayout* m_storageLocationLayout{nullptr};

	QCheckBox* m_networkStorageCheck{nullptr};
	QLabel* m_maxLabel{nullptr};
	QLabel* m_MBLabel{nullptr};
	QSlider* m_cacheSlider{nullptr};
	QLabel* m_storageLabel{nullptr};
	QLineEdit* m_storageLocationEdit{nullptr};
	QPushButton* m_changeStorageLocationButton{nullptr};

	QGridLayout* m_otherStorageLayout{nullptr};

	QCheckBox* m_savingHistoryCheck{nullptr};
	QCheckBox* m_deleteHistoryOnCloseCheck{nullptr};
	QCheckBox* m_localHTML5StorageCheck{nullptr};
	QCheckBox* m_deleteHTML5Check{nullptr};
	QPushButton* m_deleteHTML5Button{nullptr};

	QSpacerItem* m_leftSpacer{nullptr};
	QSpacerItem* m_rightSpacer{nullptr};

	QSpacerItem* m_spacer{nullptr};

};

}

#endif //SIELO_BROWSER_LOCALSTORAGEPAGE_HPP
