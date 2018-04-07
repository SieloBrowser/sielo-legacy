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

#include "Utils/DelayedFileWatcher.hpp"

#include <QTimer>

namespace Sn {

DelayedFileWatcher::DelayedFileWatcher(QObject* parent) :
	QFileSystemWatcher(parent)
{
	connect(this, &DelayedFileWatcher::directoryChanged, this, &DelayedFileWatcher::sDirectoryChanged);
	connect(this, &DelayedFileWatcher::fileChanged, this, &DelayedFileWatcher::sFileChanged);
}

DelayedFileWatcher::DelayedFileWatcher(const QStringList& paths, QObject* parent) :
	QFileSystemWatcher(paths, parent)
{
	connect(this, &DelayedFileWatcher::directoryChanged, this, &DelayedFileWatcher::sDirectoryChanged);
	connect(this, &DelayedFileWatcher::fileChanged, this, &DelayedFileWatcher::sFileChanged);
}

void DelayedFileWatcher::sDirectoryChanged(const QString& path)
{
	m_dirQueue.enqueue(path);
	QTimer::singleShot(500, this, &DelayedFileWatcher::dequeueDirectory);
}

void DelayedFileWatcher::sFileChanged(const QString& path)
{
	m_fileQueue.enqueue(path);
	QTimer::singleShot(500, this, &DelayedFileWatcher::dequeueFile);
}

void DelayedFileWatcher::dequeueDirectory()
{
	emit delayedDirectoryChanged(m_dirQueue.dequeue());
}

void DelayedFileWatcher::dequeueFile()
{
	emit delayedFileChanged(m_fileQueue.dequeue());
}

}
