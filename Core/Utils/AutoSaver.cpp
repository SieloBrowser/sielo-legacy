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

#include "Utils/AutoSaver.hpp"

#include <QDir>

#include <QCoreApplication>
#include <QMetaObject>

#include <QDebug>

namespace Sn {

AutoSaver::AutoSaver(QObject* parent) :
	QObject(parent)
{
	// Empty
}

AutoSaver::~AutoSaver()
{
	if (m_timer.isActive())
		qWarning() << "AutoSave: still active when destroyed, change not saved";
}

void AutoSaver::changeOccurred()
{
	if (m_firstChange.isNull())
		m_firstChange.start();

	if (m_firstChange.elapsed() > MAXWAIT)
		saveIfNeccessary();
	else
		m_timer.start(AUTOSAVE_IN, this);
}

void AutoSaver::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_timer.timerId())
		saveIfNeccessary();
	else
		QObject::timerEvent(event);
}

void AutoSaver::saveIfNeccessary()
{
	if (!m_timer.isActive())
		return;

	m_timer.stop();
	m_firstChange = QTime();

	if (!QMetaObject::invokeMethod(parent(), "save", Qt::DirectConnection))
		qWarning() << "AutoSaver: error invoking the parent slot for save";
}
}