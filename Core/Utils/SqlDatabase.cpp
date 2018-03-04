/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include <QtConcurrent/QtConcurrentRun>

#include <QMutexLocker>

#include <QApplication>

Q_GLOBAL_STATIC(Sn::SqlDatabase, sn_sql_database);

namespace Sn {

SqlDatabase* SqlDatabase::instance()
{
	return sn_sql_database();
}

SqlDatabase::SqlDatabase(QObject* parent) :
	QObject(parent)
{
	// Empty
}

SqlDatabase::~SqlDatabase()
{
	QMutableHashIterator<QThread*, QSqlDatabase> i(m_databases);

	while (i.hasNext()) {
		i.next();
		i.value().close();
	}
}

QSqlDatabase SqlDatabase::databaseForThread(QThread* thread)
{
	QMutexLocker lock{&m_mutex};

	if (!m_databases.contains(thread)) {
		const QString threadStr{QString::number((quintptr) thread)};

		m_databases[thread] =
			QSqlDatabase::cloneDatabase(QSqlDatabase::database(), QLatin1String("Sielo/") + threadStr);
		m_databases[thread].open();
	}

	Q_ASSERT(m_databases[thread].isOpen());

	return m_databases[thread];
}

QSqlQuery SqlDatabase::exec(QSqlQuery& query)
{
	if (QThread::currentThread() == qApp->thread()) {
		query.exec();
		return query;
	}

	QSqlQuery out{databaseForThread(QThread::currentThread())};
	out.prepare(query.lastQuery());

	const QList<QVariant> boundValues = query.boundValues().values();

		foreach (const QVariant& variant, boundValues) out.addBindValue(variant);

	out.exec();
	return query = out;
}

QFuture<QSqlQuery> SqlDatabase::execAsync(QSqlQuery& query)
{
	return QtConcurrent::run(this, &SqlDatabase::exec, query);
}

}