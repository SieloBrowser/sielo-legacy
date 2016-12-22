#pragma once

#include <QProgressBar>
#include <QLineEdit>
#include <QHBoxLayout>

class SMainWindow;

class SUrlArea : public QProgressBar
{
public:
	// Constructor and destructor
	SUrlArea(SMainWindow *parent = nullptr);
	~SUrlArea();

public slots:
	void setText(const QString& texte); //< change the text in the lineedit
	void loadStarted(); //< actions to do when page loading is started
	void loadInProgress(int percent); //< actions to do when page loading progress
    void loadFinished(); //< actions to do when page loading is finished

	void loadUrl(); //< load the url in the lineedit
private:
	SMainWindow* m_parent{ nullptr };
	QLineEdit* m_champs{ new QLineEdit(this) };

};
