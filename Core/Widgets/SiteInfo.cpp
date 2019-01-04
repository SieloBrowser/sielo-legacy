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

#include "SiteInfo.hpp"

#include <QHeaderView>

#include <QMessageBox>
#include <QFileDialog>

#include <QMenu>

#include <QClipboard>
#include <QtWidgets/qgraphicsitem.h>

#include <QWebEngine/WebProfile.hpp>

#include "Network/NetworkManager.hpp"

#include "Web/Scripts.hpp"
#include "Web/WebView.hpp"
#include "Web/WebPage.hpp"

#include "Widgets/CertificateInformation.hpp"

#include "Application.hpp"

namespace Sn {

bool SiteInfo::canShowSiteInfo(const QUrl& url)
{
	if (url.scheme() == QLatin1String("sielo") ||
		url.scheme() == QLatin1String("view-source") ||
		url.scheme() == QLatin1String("file"))
		return false;

	return true;
}

SiteInfo::SiteInfo(WebView* view) :
		QDialog(),
		m_view(view),
		m_baseUrl(view->url())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUI();

	m_siteName->setText(QString("<b>%1</b>:").arg(view->title()));
	m_siteName2->setText(QString("<b>%1</b>:").arg(view->title()));

	setupGeneral();
	setupMedia();

	m_tags->sortByColumn(-1);
	m_images->sortByColumn(-1);

	connect(m_saveButton, &QPushButton::clicked, this, &SiteInfo::saveImage);
	connect(m_images, &QTreeWidget::currentItemChanged, this, &SiteInfo::showImagePreview);
	connect(m_images, &QTreeWidget::customContextMenuRequested, this, &SiteInfo::imagesCustomContextMenuRequested);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SiteInfo::close);
}

SiteInfo::~SiteInfo()
{
	// Empty
}

void SiteInfo::showImagePreview(QTreeWidgetItem* item)
{
	if (!item)
		return;

	QUrl imageUrl = QUrl::fromEncoded(item->text(1).toUtf8());

	if (imageUrl.isRelative())
		imageUrl = m_baseUrl.resolved(imageUrl);

	QPixmap pixmap{};
	bool loading = false;

	if (imageUrl.scheme() == QLatin1String("data")) {
		QByteArray encodedUrl = item->text(1).toUtf8();
		QByteArray imageData = encodedUrl.mid(encodedUrl.indexOf(',') + 1);
		pixmap.loadFromData(QByteArray::fromBase64(imageData));
	}
	else if (imageUrl.scheme() == QLatin1String("file"))
		pixmap = QPixmap(imageUrl.toLocalFile());
	else if (imageUrl.scheme() == QLatin1String("qrc"))
		pixmap = QPixmap(imageUrl.toString().mid(3));
	else {
		delete m_imageReply;

		m_imageReply = Application::instance()->networkManager()->get(QNetworkRequest(imageUrl));

		connect(m_imageReply, &QNetworkReply::finished, this, [this]() {
			if (m_imageReply->error() != QNetworkReply::NoError)
				return;

			const QByteArray& data = m_imageReply->readAll();
			showPixmap(QPixmap::fromImage(QImage::fromData(data)));
		});

		loading = true;
		showLoadingText();
	}

	if (!loading)
		showPixmap(pixmap);
}

void SiteInfo::imagesCustomContextMenuRequested(const QPoint& point)
{
	QTreeWidgetItem* item = m_images->itemAt(point);

	if (!item)
		return;

	QMenu menu;
	menu.addAction(Application::getAppIcon("edit-copy", "edit"), tr("Copy Image Location"), this,
				   &SiteInfo::copyActionData)->setData(item->text(1));
	menu.addAction(tr("Copy Image Name"), this, &SiteInfo::copyActionData)->setData(item->text(0));
	menu.addSeparator();
	menu.addAction(tr("Save Image to Disk"), this, &SiteInfo::saveImage);

	menu.exec(m_images->viewport()->mapToGlobal(point));
}

void SiteInfo::copyActionData()
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		Application::instance()->clipboard()->setText(action->data().toString());
}

void SiteInfo::saveImage()
{
	QTreeWidgetItem* item = m_images->currentItem();

	if (!item)
		return;

	if (m_preview->scene() || m_preview->scene()->items().isEmpty())
		return;

	QGraphicsItem* graphicsItem = m_preview->scene()->items()[0];
	QGraphicsPixmapItem* pixmapItem = static_cast<QGraphicsPixmapItem*>(graphicsItem);

	if (graphicsItem->type() != QGraphicsPixmapItem::Type || !pixmapItem)
		return;

	if (!pixmapItem || pixmapItem->pixmap().isNull()) {
		QMessageBox::critical(this, tr("Error"), tr("This preview is not available!"));
		return;
	}

	QString imageFileName = QUrl(item->text(1)).toString(
			QUrl::RemoveFragment | QUrl::RemoveQuery | QUrl::RemoveScheme | QUrl::RemovePort);

	if (imageFileName.endsWith(QLatin1Char('/'))) {
		imageFileName = imageFileName.mid(0, imageFileName.length() - 1);
	}

	if (imageFileName.indexOf(QLatin1Char('/')) != -1) {
		int pos = imageFileName.lastIndexOf(QLatin1Char('/'));
		imageFileName = imageFileName.mid(pos);
		imageFileName.remove(QLatin1Char('/'));
	}

	if (imageFileName.isEmpty()) {
		imageFileName = QUrl(item->text(1)).host();
	}

	imageFileName.replace(QLatin1Char('/'), QLatin1Char('-'));
	imageFileName.remove(QLatin1Char('\\'));
	imageFileName.remove(QLatin1Char(':'));
	imageFileName.remove(QLatin1Char('*'));
	imageFileName.remove(QLatin1Char('?'));
	imageFileName.remove(QLatin1Char('"'));
	imageFileName.remove(QLatin1Char('<'));
	imageFileName.remove(QLatin1Char('>'));
	imageFileName.remove(QLatin1Char('|'));

	int index{imageFileName.lastIndexOf(QLatin1Char('.'))};
	if (index != -1) {
		imageFileName = imageFileName.left(index);
		imageFileName.append(QLatin1String(".png"));
	}

	QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image..."),
													QDir::homePath() + QDir::separator() + imageFileName,
													QStringLiteral("*.png"));

	if (filePath.isEmpty())
		return;

	if (!pixmapItem->pixmap().save(filePath, "PNG")) {
		QMessageBox::critical(this, tr("Error"), tr("Cannot write to file!"));
		return;
	}
}

void SiteInfo::showLoadingText()
{
	delete m_preview->scene();

	QGraphicsScene* scene{new QGraphicsScene(m_preview)};

	scene->addText(tr("Loading..."));
	m_preview->setScene(scene);
}

void SiteInfo::showPixmap(QPixmap pixmap)
{
	pixmap.setDevicePixelRatio(devicePixelRatio());

	delete m_preview->scene();

	QGraphicsScene* scene{new QGraphicsScene(m_preview)};

	if (pixmap.isNull())
		scene->addText(tr("Preview not available"));
	else
		scene->addPixmap(pixmap);

	m_preview->setScene(scene);
}

void SiteInfo::setupGeneral()
{
	m_siteAddress->setText(m_view->url().toString());

	m_view->page()->runJavaScript(QStringLiteral("document.charset"), Engine::WebProfile::ScriptWorldId::ApplicationWorld,
								  [this](const QVariant& res) {
									  m_encoding->setText(res.toString());
								  });

	m_view->page()->runJavaScript(Scripts::getAllMetaAttributes(), Engine::WebProfile::ScriptWorldId::ApplicationWorld,
								  [this](const QVariant& res) {
									  const QVariantList& list = res.toList();

											  foreach (const QVariant& val, list) {
											  const QVariantMap& meta = val.toMap();
											  QString content = meta.value(QStringLiteral("content")).toString();
											  QString name = meta.value(QStringLiteral("name")).toString();

											  if (name.isEmpty())
												  name = meta.value(QStringLiteral("httpequiv")).toString();

											  if (content.isEmpty() || name.isEmpty())
												  continue;

											  QTreeWidgetItem* item{new QTreeWidgetItem(m_tags)};
											  item->setText(0, name);
											  item->setText(1, content);

											  m_tags->addTopLevelItem(item);
										  }
								  });

	if (m_view->url().scheme() == QLatin1String("https"))
		m_security->setText(tr("Connection Encrypted"));
	else
		m_security->setText(tr("Connection Not Encrypted"));
}

void SiteInfo::setupMedia()
{
	m_view->page()->runJavaScript(Scripts::getAllImages(), Engine::WebProfile::ScriptWorldId::ApplicationWorld,
								  [this](const QVariant& res) {
									  const QVariantList& list = res.toList();

											  foreach (const QVariant& val, list) {
											  const QVariantMap& img = val.toMap();
											  QString src = img.value(QStringLiteral("src")).toString();
											  QString alt = img.value(QStringLiteral("alt")).toString();

											  if (alt.isEmpty()) {
												  if (src.indexOf(QLatin1Char('/')) == -1)
													  alt = src;
												  else {
													  int pos{src.lastIndexOf(QLatin1Char('/'))};
													  alt = src.mid(pos);
													  alt.remove(QLatin1Char('/'));
												  }
											  }

											  if (src.isEmpty() || alt.isEmpty())
												  continue;

											  QTreeWidgetItem* item{new QTreeWidgetItem(m_images)};
											  item->setText(0, alt);
											  item->setText(1, src);

											  m_images->addTopLevelItem(item);
										  }
								  });
}

void SiteInfo::setupUI()
{
	resize(600, 490);

	m_layout = new QVBoxLayout{this};
	m_tabs = new QTabWidget(this);
	m_generalPage = new QWidget(this);
	m_mediaPage = new QWidget(this);
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);

	// General page
	m_generalLayout = new QGridLayout(m_generalPage);
	m_infoLaytout = new QFormLayout();

	m_siteName = new QLabel(m_generalPage);

	m_descSiteAddress = new QLabel(tr("Site address"), m_generalPage);
	m_siteAddress = new EllipseLabel(m_generalPage);
	m_descEncoding = new QLabel(tr("Encoding"), m_generalPage);
	m_encoding = new EllipseLabel(m_generalPage);

	m_infoLaytout->setWidget(0, QFormLayout::LabelRole, m_descSiteAddress);
	m_infoLaytout->setWidget(0, QFormLayout::FieldRole, m_siteAddress);
	m_infoLaytout->setWidget(1, QFormLayout::LabelRole, m_descEncoding);
	m_infoLaytout->setWidget(1, QFormLayout::FieldRole, m_encoding);

	m_descMetaTags = new QLabel(tr("Meta tags of site:"), m_generalPage);
	m_tags = new QTreeWidget(m_generalPage);

	m_tags->setSortingEnabled(true);
	m_tags->header()->setMinimumSectionSize(200);
	m_tags->headerItem()->setText(0, tr("Tag"));
	m_tags->headerItem()->setText(1, tr("Value"));

	m_descSecurity = new QLabel(tr("<b>Security information</b>"), m_generalPage);
	m_security = new QLabel(m_generalPage);

	m_generalLayout->addWidget(m_siteName, 0, 0, 1, 2);
	m_generalLayout->addLayout(m_infoLaytout, 1, 0, 1, 2);
	m_generalLayout->addWidget(m_descMetaTags, 2, 0, 1, 2);
	m_generalLayout->addWidget(m_tags, 3, 0, 1, 2);
	m_generalLayout->addWidget(m_descSecurity, 4, 0, 1, 1);
	m_generalLayout->addWidget(m_security, 4, 1, 1, 1);

	// Media page
	m_mediaLayout = new QVBoxLayout(m_mediaPage);
	m_previewLayout = new QVBoxLayout();
	m_splitter = new QSplitter(Qt::Vertical, m_mediaPage);

	m_siteName2 = new QLabel(m_mediaPage);

	m_images = new QTreeWidget(m_mediaPage);

	m_images->setItemsExpandable(false);
	m_images->setSortingEnabled(true);
	m_images->header()->setMinimumSectionSize(200);
	m_images->headerItem()->setText(0, tr("Image"));
	m_images->headerItem()->setText(1, tr("Image address"));
	m_images->setContextMenuPolicy(Qt::CustomContextMenu);

	m_saveButton = new QPushButton(tr("Save"), m_mediaPage);
	m_descPreview = new QLabel(tr("<b>Preview</b>"), m_mediaPage);
	m_preview = new QGraphicsView(m_mediaPage);

	m_previewLayout->addWidget(m_saveButton);
	m_previewLayout->addWidget(m_descPreview);
	m_previewLayout->addWidget(m_preview);

	QWidget* layoutWidget{new QWidget(this)};
	layoutWidget->setLayout(m_previewLayout);

	m_splitter->addWidget(m_images);
	m_splitter->addWidget(layoutWidget);

	m_mediaLayout->addWidget(m_siteName2);
	m_mediaLayout->addWidget(m_splitter);

	// Add page to tabs
	m_tabs->addTab(m_generalPage, tr("General"));
	m_tabs->addTab(m_mediaPage, tr("Media"));

	// Add widgets to layout
	m_layout->addWidget(m_tabs);
	m_layout->addWidget(m_buttonBox);
}
}