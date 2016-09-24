#include "../includes/SUrlArea.hpp"
#include "../includes/SMainWindow.hpp"

SUrlArea::SUrlArea(SMainWindow * parent) :
	QProgressBar(parent),
	m_parent(parent)
{
	QHBoxLayout* layout{ new QHBoxLayout(this) };
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_champs->setStyleSheet("background: rgba(255, 255, 255, 0)");
	m_champs->setClearButtonEnabled(true);
	layout->addWidget(m_champs);

	setTextVisible(false);
}

void SUrlArea::setText(const QString& texte)
{
	m_champs->setText(texte);
}

void SUrlArea::loadStarted()
{
}

void SUrlArea::loadInProgress(int percent)
{
	setStyleSheet("QProgressBar::chunk{background-color: rgba(0, 0, 155, 0.2)}");
	setValue(percent);
}

void SUrlArea::loadFinished(bool ok)
{
}
