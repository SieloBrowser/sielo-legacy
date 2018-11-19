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
#ifndef SIELO_BROWSER_PASSWORDBACKEND_HPP
#define SIELO_BROWSER_PASSWORDBACKEND_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QVector>

#include "Password/PasswordManager.hpp"

namespace Sn {
class SIELO_SHAREDLIB PasswordBackend {
public:
	PasswordBackend();
	virtual ~PasswordBackend();

	virtual QString name() const = 0;

	virtual QVector<PasswordEntry> getEntries(const QUrl& url) = 0;
	virtual QVector<PasswordEntry> getAllEntries() = 0;

	virtual void addEntry(const PasswordEntry& entry) = 0;
	virtual bool updateEntry(const PasswordEntry& entry) = 0;
	virtual void updateLastUsed(PasswordEntry& entry) = 0;

	virtual void removeEntry(const PasswordEntry& entry) = 0;
	virtual void removeAll() = 0;

	virtual void setActive(bool active);
	bool isActive() const { return m_active; }

	virtual bool hasSettings() const;
	virtual void showSettings(QWidget* parent);
private:
	bool m_active{false};
};

}

#endif //SIELO_BROWSER_PASSWORDBACKEND_HPP
