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
#ifndef SIELOBROWSER_ADBSEARCHTREE_HPP
#define SIELOBROWSER_ADBSEARCHTREE_HPP

#include "SharedDefines.hpp"

#include <QChar>
#include <QHash>

#include <QWebEngine/UrlRequestInfo.hpp>

namespace Sn {
namespace ADB {
class Rule;

class SIELO_SHAREDLIB SearchTree {
public:
	SearchTree();
	~SearchTree();

	void clear();

	bool add(const Rule* rule);
	const Rule* find(const Engine::UrlRequestInfo& request, const QString& domain, const QString& urlString) const;

private:
	struct Node {
		QChar c{};
		const Rule* rule{nullptr};
		QHash<QChar, Node*> children;

		Node() :
				c(0),
				rule(nullptr) {}
	};

	const Rule* prefixSearch(const Engine::UrlRequestInfo& request, const QString& domain, const QString& urlString,
							 const QChar* string, int length) const;

	void deleteNode(Node* node);

	Node* m_root{nullptr};
};

}
}

#endif //SIELOBROWSER_ADBSEARCHTREE_HPP
