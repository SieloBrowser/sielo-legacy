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

#include "ContextMenuData.hpp"

namespace Engine {
ContextMenuData::ContextMenuData(QWebEngineContextMenuData* data) :
	QObject(),
	m_data(data)
{
	// Empty
}

bool ContextMenuData::isValid() const
{
	return m_data->isValid();
}

QString ContextMenuData::selectedText() const
{
	return m_data->selectedText();
}

QString ContextMenuData::linkText() const
{
	return m_data->linkText();
}

QUrl ContextMenuData::linkUrl() const
{
	return m_data->linkUrl();
}

QUrl ContextMenuData::mediaUrl() const
{
	return m_data->mediaUrl();
}

bool ContextMenuData::isContentEditable() const
{
	return m_data->isContentEditable();
}

QString ContextMenuData::misspelledWord() const
{
	return m_data->misspelledWord();
}

QStringList ContextMenuData::spellCheckerSuggestions() const
{
	return m_data->spellCheckerSuggestions();
}

ContextMenuData::MediaType ContextMenuData::mediaType() const
{
	return static_cast<MediaType>(m_data->mediaType());
}

ContextMenuData::MediaFlags ContextMenuData::mediaFlags() const
{
	switch (m_data->mediaFlags()) {
	case QWebEngineContextMenuData::MediaInError:
		return MediaFlag::MediaInError;
	case QWebEngineContextMenuData::MediaPaused:
		return MediaFlag::MediaPaused;
	case QWebEngineContextMenuData::MediaMuted:
		return MediaFlag::MediaMuted;
	case QWebEngineContextMenuData::MediaLoop:
		return MediaFlag::MediaLoop;
	case QWebEngineContextMenuData::MediaCanSave:
		return MediaFlag::MediaCanSave;
	case QWebEngineContextMenuData::MediaHasAudio:
		return MediaFlag::MediaHasAudio;
	case QWebEngineContextMenuData::MediaCanToggleControls:
		return MediaFlag::MediaCanToggleControls;
	case QWebEngineContextMenuData::MediaControls:
		return MediaFlag::MediaControls;
	case QWebEngineContextMenuData::MediaCanPrint:
		return MediaFlag::MediaCanPrint;
	case QWebEngineContextMenuData::MediaCanRotate:
		return MediaFlag::MediaCanRotate;
	}
}
}
