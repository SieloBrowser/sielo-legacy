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
#ifndef SIELOBROWSER_SITEINFO_HPP
#define SIELOBROWSER_SITEINFO_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QDialog>

#include <QGridLayout>
#include <QFormLayout>
#include <QVBoxLayout>

#include <QDialogButtonBox>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QGraphicsView>
#include <QTreeWidget>

#include <QNetworkReply>

#include "Widgets/EllipseLabel.hpp"

namespace Sn {
class WebView;

class CertificateInformation;

class SIELO_SHAREDLIB SiteInfo : public QDialog {
Q_OBJECT

public:
	SiteInfo(WebView* view);
	~SiteInfo();

	static bool canShowSiteInfo(const QUrl& url);

private slots:
	void showImagePreview(QTreeWidgetItem* item);
	void imagesCustomContextMenuRequested(const QPoint& point);
	void copyActionData();
	void saveImage();

private:
	void setupUI();
	void setupGeneral();
	void setupMedia();

	void showLoadingText();
	void showPixmap(QPixmap pixmap);

	CertificateInformation* m_certificate{nullptr};
	WebView* m_view{nullptr};
	QNetworkReply* m_imageReply{nullptr};

	QUrl m_baseUrl{};

	// UI
	QVBoxLayout* m_layout{nullptr};
	QTabWidget* m_tabs{nullptr};
	QWidget* m_generalPage{nullptr};
	QWidget* m_mediaPage{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};

	// General page
	QGridLayout* m_generalLayout{nullptr};
	QFormLayout* m_infoLaytout{nullptr};
	QLabel* m_siteName{nullptr};

	QLabel* m_descSiteAddress{nullptr};
	EllipseLabel* m_siteAddress{nullptr};
	QLabel* m_descEncoding{nullptr};
	EllipseLabel* m_encoding{nullptr};

	QLabel* m_descMetaTags{nullptr};
	QTreeWidget* m_tags{nullptr};

	QLabel* m_descSecurity{nullptr};
	QLabel* m_security{nullptr};

	// Media page
	QVBoxLayout* m_mediaLayout{nullptr};
	QSplitter* m_splitter{nullptr};
	QVBoxLayout* m_previewLayout{nullptr};
	QLabel* m_siteName2{nullptr};
	QTreeWidget* m_images{nullptr};
	QPushButton* m_saveButton{nullptr};
	QLabel* m_descPreview{nullptr};
	QGraphicsView* m_preview{nullptr};


};

}

#endif //SIELOBROWSER_SITEINFO_HPP
