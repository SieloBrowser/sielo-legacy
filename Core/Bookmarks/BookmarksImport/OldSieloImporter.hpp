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
#ifndef SIELOBROWSER_OLDSIELOIMPORTER_HPP
#define SIELOBROWSER_OLDSIELOIMPORTER_HPP

#include "SharedDefines.hpp"

#include "BookmarksImporter.hpp"

#include <QFile>
#include <QXmlStreamReader>

namespace Sn
{
class SIELO_SHAREDLIB OldSieloImporter: public BookmarksImporter {
	Q_OBJECT

public:
	OldSieloImporter(QObject* parent = nullptr);
	~OldSieloImporter();

	QString description() const;
	QString standardPath() const;

	QString getPath(QWidget* parent);
	bool prepareImport();

	BookmarkItem* importBookmarks();

private:
	void readXBEL(BookmarkItem* root);
	void readTitle(BookmarkItem *parent);
	void readDescription(BookmarkItem *parent);
	void readSeparator(BookmarkItem *parent);
	void readFolder(BookmarkItem *parent);
	void readBookmarkNode(BookmarkItem *parent);

	QXmlStreamReader* m_reader{ nullptr };
	QString m_path;
	QFile m_file;
}
;
}

#endif //SIELOBROWSER_OLDSIELOIMPORTER_HPP