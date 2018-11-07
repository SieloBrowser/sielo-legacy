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

#include "UpdaterDialog.hpp"

#include <QStandardPaths>
#include <QDesktopServices>

#include <QIcon>

#include <QMessageBox>

#include "Network/NetworkManager.hpp"

#include "Application.hpp"

namespace Sn {
UpdaterDialog::UpdaterDialog(QWidget* parent) :
	QDialog(parent)
{
	setObjectName(QLatin1String("updater-dialog"));
	setAttribute(Qt::WA_DeleteOnClose);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);
	setWindowFlag(Qt::WindowStaysOnTopHint);

	setupUI();
	show();
}

UpdaterDialog::~UpdaterDialog()
{
	// Empty
}

void UpdaterDialog::downloadProgress(qint64 ist, qint64 max)
{
	m_progress->setRange(0, max);
	m_progress->setValue(ist);
}

void UpdaterDialog::downloadCompleted()
{
	close();
}

void UpdaterDialog::setupUI()
{
	m_layout = new QVBoxLayout(this);
	m_information = new QLabel(tr("Sielo is making an update. Sielo will restart at the end of the download."), this);
	m_progress = new QProgressBar(this);

	m_layout->addWidget(m_information);
	m_layout->addWidget(m_progress);
}

}