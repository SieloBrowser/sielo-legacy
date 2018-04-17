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

#include "Application.hpp"

#include <QMessageBox>

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <iostream>

Application::Application(int& argc, char** argv) :
	QApplication(argc, argv)
{
	if (argc > 1) {
		QStringList args{QCoreApplication::arguments()};

		if (args[1] == "compile") {
			if (args[2] == "theme") {
				if (!compile(args[3], args[4] + QLatin1String(".snthm")))
					QMessageBox::critical(nullptr,
										  QApplication::tr("Error"),
										  QApplication::tr("Failed to compile theme: ") + m_errors);
				else
					QMessageBox::information(nullptr,
											 QApplication::tr("Success"),
											 QApplication::tr("Theme compiled with success"));
			}
			else
				QMessageBox::critical(nullptr,
									  QApplication::tr("Error"),
									  QApplication::tr("%1 is not a valid Sielo format").arg(args[2]));
		}
		else if (args[1] == "decompile") {
			if (!decompile(args[2], args[3]))
				QMessageBox::critical(nullptr,
									  QApplication::tr("Error"),
									  QApplication::tr("Failed to decompile: ") + m_errors);
			else if (args.count() == 5) {
				QMessageBox::information(nullptr, QApplication::tr("Success"), args[4]);
			}
		}
		else if (args[1] == "-h" || args[1] == "--help") {
			std::cout << QApplication::tr("Their is two ways to use this command:").toStdString() << std::endl << std::endl;
			std::cout << QApplication::tr("$ sielo-compiler compile theme (path to the theme folder) (name of the theme)").toStdString() << std::endl;
			std::cout << " -> " << QApplication::tr("This will compile your theme in a basic \".sntm\" file. The output is in the theme folder directory.").toStdString() << std::endl << std::endl;
			std::cout << QApplication::tr("$ sielo-compiler decompile (path to the theme file) (path where the theme must be decompiled)").toStdString() << std::endl;
			std::cout << " -> " << QApplication::tr("This will decompile your theme in the directory you choose.").toStdString() << std::endl << std::endl;

		}
		else
			QMessageBox::critical(nullptr,
								  QApplication::tr("Error"),
								  QApplication::tr("The action you want to do is unknown..."));
	}

	QCoreApplication::instance()->exit(0);
}

Application::~Application()
{
	// Empty
}

bool Application::compile(const QString& srcFolder, const QString& fileDestination)
{
	QDir src{srcFolder};

	if (!src.exists()) {
		m_errors = QApplication::tr("The folder to compile doesn't exist.");
		return false;
	}

	m_file.setFileName(fileDestination);

	if (!m_file.open(QIODevice::WriteOnly)) {
		m_errors = QApplication::tr("The destination file can't be open.");
		return false;
	}

	m_stream.setDevice(&m_file);

	bool success{compress(srcFolder, "")};

	m_file.close();

	return success;

}

bool Application::decompile(const QString& srcFile, const QString& filesDestination)
{
	QFile src{srcFile};
	QFileInfo name{};

	if (!src.exists()) {
		m_errors = QApplication::tr("Sources to decompile don't exists.");
		return false;
	}

	QDir dir{};

	if (!dir.mkpath(filesDestination)) {
		m_errors = QApplication::tr("Can't create folder to receive decompiled files");
		return false;
	}

	if (!src.open(QIODevice::ReadOnly)) {
		m_errors = QApplication::tr("Failed to read compiled file.");
		return false;
	}

	QDataStream stream{&src};

	while (!stream.atEnd()) {
		QString fileName{};
		QByteArray data{};

		stream >> fileName >> data;

		QString subFolder{};

		for (int i{fileName.length() - 1}; i > 0; --i) {
			if ((QString(fileName[i]) == QString("\\")) || (QString(fileName[i]) == QString("/"))) {
				subFolder = fileName.left(i);
				dir.mkpath(filesDestination + QLatin1Char('/') + subFolder);

				break;
			}
		}

		QFile outFile{filesDestination + QLatin1Char('/') + fileName};

		if (!outFile.open(QIODevice::WriteOnly)) {
			src.close();
			m_errors = QApplication::tr("Failed to write decompiled files.");
			return false;
		}

		outFile.write(qUncompress(data));
		outFile.close();
	}

	src.close();

	return true;
}

bool Application::compress(const QString& srcFolder, const QString& prefexe)
{
	QDir dir{srcFolder};

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

	QFileInfoList folderList{dir.entryInfoList()};

	for (int i{0}; i < folderList.length(); ++i) {
		QString folderName{folderList[i].fileName()};
		QString folderPath{dir.absolutePath() + QLatin1Char('/') + folderName};
		QString newPrefexe{prefexe + QLatin1Char('/') + folderName};

		compress(folderPath, newPrefexe);
	}

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

	QFileInfoList filesList{dir.entryInfoList()};

	for (int i{0}; i < filesList.length(); ++i) {
		QFile file{dir.absolutePath() + QLatin1Char('/') + filesList[i].fileName()};

		if (!file.open(QIODevice::ReadOnly)) {
			m_errors = QApplication::tr("Failed to read ") + filesList[i].fileName();
			return false;
		}

		m_stream << QString(prefexe + QLatin1Char('/') + filesList[i].fileName());
		m_stream << qCompress(file.readAll());

		file.close();
	}

	return true;
}