#pragma once

#include <QProgressBar>
#include <QLineEdit>
#include <QHBoxLayout>

class SMainWindow;

class SUrlArea : public QProgressBar
{
public:
	SUrlArea(SMainWindow *parent = nullptr);

public slots:
	void setText(const QString& texte);
	void loadStarted();
	void loadInProgress(int percent);
    void loadFinished();

	void loadUrl();

private:
	SMainWindow* m_parent{ nullptr };
	QLineEdit* m_champs{ new QLineEdit(this) };

};
