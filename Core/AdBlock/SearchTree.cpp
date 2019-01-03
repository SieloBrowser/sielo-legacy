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

#include "AdBlock/SearchTree.hpp"

#include <QtDebug>

#include "AdBlock/Rule.hpp"

namespace Sn {
namespace ADB {

SearchTree::SearchTree() :
		m_root(new Node)
{
	// Empty
}

SearchTree::~SearchTree()
{
	deleteNode(m_root);
}

void SearchTree::clear()
{
	deleteNode(m_root);
	m_root = new Node;
}

bool SearchTree::add(const Rule* rule)
{
	if (rule->m_type != Rule::StringContainsMatchRule)
		return false;

	const QString filter{rule->m_matchString};
	int length{filter.size()};

	if (length <= 0) {
		qDebug() << "ADB::SearchTree: Inserting rule with filter length <= 0!";
		return false;
	}

	Node* node = m_root;

	for (const QChar c : filter) {
		if (!node->children.contains(c)) {
			Node* n{new Node};
			n->c = c;

			node->children[c] = n;
		}

		node = node->children[c];
	}

	node->rule = rule;

	return true;
}

const Rule* SearchTree::find(const Engine::UrlRequestInfo& request, const QString& domain,
							 const QString& urlString) const
{
	int length{urlString.size()};

	if (length <= 0)
		return nullptr;

	const QChar* string{urlString.constData()};

	for (int i{0}; i < length; ++i) {
		const Rule* rule{prefixSearch(request, domain, urlString, ++string, length - i)};
		if (rule)
			return rule;
	}

	return nullptr;
}

const Rule* SearchTree::prefixSearch(const Engine::UrlRequestInfo& request, const QString& domain,
									 const QString& urlString, const QChar* string, int length) const
{
	if (length <= 0)
		return nullptr;

	QChar c{string[0]};

	if (!m_root->children.contains(c))
		return nullptr;

	Node* node{m_root->children[c]};

	for (int i{1}; i < length; ++i) {
		const QChar c{(++string)[0]};

		if (node->rule && node->rule->networkMatch(request, domain, urlString))
			return node->rule;
		if (!node->children.contains(c))
			return nullptr;

		node = node->children[c];
	}

	if (node->rule && node->rule->networkMatch(request, domain, urlString))
		return node->rule;

	return nullptr;
}

void SearchTree::deleteNode(Node* node)
{
	if (!node)
		return;

	QHashIterator<QChar, Node*> iterator{node->children};

	while (iterator.hasNext()) {
		iterator.next();
		deleteNode(iterator.value());
	}

	delete node;
};

}
}