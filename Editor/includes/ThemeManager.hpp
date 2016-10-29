#pragma once

#include <QFile>
#include <QDataStream>

// This is the class for compress and decompress Sielo themes
class ThemeManager : public QObject
{
public:
	// Constructor and destructor
	static bool compressTheme(QString srcFolder, QString fileDestionation);
	static bool decompressTheme(QString srcTheme, QString dst);

	static ThemeManager *mgr;
private:
	QFile m_file{};
	QDataStream m_dataStream{};

	static bool compress(QString srcFolder, QString prefex);
};