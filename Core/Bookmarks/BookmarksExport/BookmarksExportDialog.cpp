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

#include "BookmarksExportDialog.hpp"

#include <QMessageBox>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarksExport/BookmarksExporter.hpp"
#include "Bookmarks/BookmarksExport/HtmlExporter.hpp"

#include "Application.hpp"

namespace Sn
{
BookmarksExportDialog::BookmarksExportDialog(QWidget* parent):
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("bookmarks-dialog-expot"));
	setWindowTitle(tr("Bookmarks Exporter"));
	setupUI();

	m_exporters.append(new HtmlExporter());

	foreach(BookmarksExporter* exporter, m_exporters)
		m_format->addItem(exporter->name());

	m_currentExporter = m_exporters[0];

	connect(m_chooseOutput, &QPushButton::clicked, this, &BookmarksExportDialog::setPath);
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &BookmarksExportDialog::exportBookmarks);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &BookmarksExportDialog::close);
}

BookmarksExportDialog::~BookmarksExportDialog()
{
	// Empty
}

void BookmarksExportDialog::setPath()
{
	Q_ASSERT(m_currentExporter);

	m_output->setText(m_currentExporter->getPath(this));
}

void BookmarksExportDialog::exportBookmarks()
{
	Q_ASSERT(m_currentExporter);

	if (m_output->text().isEmpty())
		return;

	bool ok{m_currentExporter->exportBookmarks(Application::instance()->bookmarks()->rootItem())};

	if (!ok)
		QMessageBox::critical(this, tr("Error!"), m_currentExporter->errorString());
	else
		close();
}

void BookmarksExportDialog::setupUI()
{
	resize(570, 170);

	m_box = new QGroupBox(tr("Export options"), this);

	m_layout = new QVBoxLayout(this);
	m_exportLayout = new QFormLayout(m_box);
	m_pathLayout = new QHBoxLayout();

	m_desc = new QLabel(tr("Export Bookmarks"), this);

	m_formatDesc = new QLabel(tr("Format:"), this);
	m_format = new QComboBox(this);

	m_outputDesc = new QLabel(tr("Output File:"), this);
	m_output = new QLineEdit(this);
	m_chooseOutput = new QPushButton(tr("Choose..."), this);

	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

	m_pathLayout->addWidget(m_output);
	m_pathLayout->addWidget(m_chooseOutput);

	m_exportLayout->setWidget(0, QFormLayout::LabelRole, m_formatDesc);
	m_exportLayout->setWidget(0, QFormLayout::FieldRole, m_format);
	m_exportLayout->setWidget(1, QFormLayout::LabelRole, m_outputDesc);
	m_exportLayout->setLayout(1, QFormLayout::FieldRole, m_pathLayout);

	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_box);
	m_layout->addWidget(m_buttonBox);
}
}
