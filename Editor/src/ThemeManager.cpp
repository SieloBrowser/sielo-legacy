#include "includes/ThemeManager.hpp"

#include <QDir>
#include <QFileInfo>

ThemeManager *ThemeManager::mgr = new ThemeManager;
bool ThemeManager::compressTheme(QString srcFolder, QString fileDestionation)
{
	QDir src{ srcFolder };
	if (!src.exists())
		return false;

	mgr->m_file.setFileName(fileDestionation);
	if (!mgr->m_file.open(QIODevice::WriteOnly))
		return false;

	mgr->m_dataStream.setDevice(&mgr->m_file);

	bool success{ compress(srcFolder, "") };
	mgr->m_file.close();

	return success;
}

bool ThemeManager::decompressTheme(QString srcTheme, QString dst)
{
	QFile src{ srcTheme };
	QFileInfo themeName{ src };
	QString destinationFolder{ dst };

	if (!src.exists())
		return false;

	QDir dir{};
	if (!dir.mkpath(destinationFolder))
		return false;

	mgr->m_file.setFileName(srcTheme);
	if (!mgr->m_file.open(QIODevice::ReadOnly))
		return false;

	mgr->m_dataStream.setDevice(&mgr->m_file);

	while (!mgr->m_dataStream.atEnd()) {
		QString fileName{};
		QByteArray data{};

		mgr->m_dataStream >> fileName >> data;

		QString subFolder{};
		for (int i{ fileName.length() - 1 }; i > 0; --i) {
			if ((QString(fileName[i]) == QString("\\")) || (QString(fileName[i]) == QString("/"))) {
				subFolder = fileName.left(i);
				dir.mkpath(destinationFolder + "/" + subFolder);
				break;
			}
		}

		QFile outFile(destinationFolder + "/" + fileName);
		if (!outFile.open(QIODevice::WriteOnly)) {
			mgr->m_file.close();
			return false;
		}

		outFile.write(qUncompress(data));
		outFile.close();
	}

	mgr->m_file.close();

	return true;
}

bool ThemeManager::compress(QString srcFolder, QString prefix)
{
	QDir dir{ srcFolder };
	if (!dir.exists())
		return false;

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	QFileInfoList folderList{ dir.entryInfoList() };

	for (int i{ 0 }; i < folderList.length(); ++i) {
		QString folderName{ folderList[i].fileName() };
		QString folderPath{ dir.absolutePath() + "/" + folderName };
		QString newPrefex{ prefix + "/" + folderName };

		compress(folderPath, newPrefex);
	}

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
	QFileInfoList filesList{ dir.entryInfoList() };

	for (int i{ 0 }; i < filesList.length(); ++i) {
		QFile file{ dir.absolutePath() + "/" + filesList[i].fileName() };

		if (!file.open(QIODevice::ReadOnly))
			return false;

		mgr->m_dataStream << QString(prefix + "/" + filesList[i].fileName());
		mgr->m_dataStream << qCompress(file.readAll());

		file.close();
	}

	return true;
}