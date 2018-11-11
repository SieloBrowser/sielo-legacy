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

#include "SqlDatabase.hpp"

#include <QThread>
#include <QThreadStorage>

#include <QCoreApplication>

namespace  Sn
{

QThreadStorage<QSqlDatabase> s_databases;
Q_GLOBAL_STATIC(SqlDatabase, sn_sql_database);

SqlDatabase *SqlDatabase::instance() {
	return sn_sql_database();
}

SqlDatabase::SqlDatabase(QObject* parent) :
	QObject(parent) {
	// Empty
}

QSqlDatabase SqlDatabase::database() const {
	if (QThread::currentThread() == QCoreApplication::instance()->thread())
		return QSqlDatabase::database();

	if (!s_databases.hasLocalData()) {
		const QString threadStr{QString::number(reinterpret_cast<quintptr>(QThread::currentThread()))};
		QSqlDatabase database{QSqlDatabase::addDatabase("QSQLITE", threadStr)};

		database.setDatabaseName(m_databaseName);
		database.setConnectOptions(m_connectOptions);
		database.open();

		s_databases.setLocalData(database);
	}

	return s_databases.localData();
}

void SqlDatabase::setDatabase(const QSqlDatabase& database) {
	m_databaseName = database.databaseName();
	m_connectOptions = database.connectOptions();
}

}
