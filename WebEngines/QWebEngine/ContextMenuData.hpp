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

#ifndef SIELO_BROWSER_CONTEXTMENUDATA_HPP
#define SIELO_BROWSER_CONTEXTMENUDATA_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QtWebEngineWidgets/QWebEngineContextMenuData>

namespace Engine
{
class SIELO_SHAREDLIB ContextMenuData: public QObject {
	Q_OBJECT

public:
	enum MediaType {
		MediaTypeNone = 0,
		MediaTypeImage = 1,
		MediaTypeVideo = 2,
		MediaTypeAudio = 3,
		MediaTypeCanvas = 4,
		MediaTypeFile = 5,
		MediaTypePlugin = 6
	};

	// Must match QWebEngineCore::WebEngineContextMenuData::MediaFlags:
	enum MediaFlag {
		MediaInError = 0x1,
		MediaPaused = 0x2,
		MediaMuted = 0x4,
		MediaLoop = 0x8,
		MediaCanSave = 0x10,
		MediaHasAudio = 0x20,
		MediaCanToggleControls = 0x40,
		MediaControls = 0x80,
		MediaCanPrint = 0x100,
		MediaCanRotate = 0x200,
	};
	Q_DECLARE_FLAGS(MediaFlags, MediaFlag)
		Q_FLAG(MediaFlags)

		// Must match QWebEngineCore::WebEngineContextMenuData::EditFlags:
		enum EditFlag {
		CanUndo = 0x1,
		CanRedo = 0x2,
		CanCut = 0x4,
		CanCopy = 0x8,
		CanPaste = 0x10,
		CanDelete = 0x20,
		CanSelectAll = 0x40,
		CanTranslate = 0x80,
		CanEditRichly = 0x100,
	};
	Q_DECLARE_FLAGS(EditFlags, EditFlag)
		Q_FLAG(EditFlags)

		ContextMenuData(QWebEngineContextMenuData* data);
	~ContextMenuData() = default;

	bool isValid() const;

	QString selectedText() const;
	QString linkText() const;
	QUrl linkUrl() const;
	QUrl mediaUrl() const;
	bool isContentEditable() const;

	QString misspelledWord() const;
	QStringList spellCheckerSuggestions() const;

	MediaType mediaType() const;
	MediaFlags mediaFlags() const;

private:
	QWebEngineContextMenuData* m_data{nullptr};
};
}

#endif //SIELO_BROWSER_CONTEXTMENUDATA_HPP
