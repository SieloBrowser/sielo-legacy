#include "../includes/SUrlArea.hpp"
#include "../includes/SMainWindow.hpp"
#include "../includes/Actions.hpp"

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

	connect(m_champs, &QLineEdit::returnPressed, this, &SUrlArea::loadUrl);
}

void SUrlArea::setText(const QString& texte)
{
	m_champs->setText(texte);
}

void SUrlArea::loadStarted()
{
	m_parent->getActions()->refreshOrStop->setIcon(QIcon(m_parent->getActions()->themePath + "stop.png"));
	m_parent->getActions()->refreshOrStop->setText("Arrêter le chargement");
	connect(m_parent->getActions()->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::stop);
}

void SUrlArea::loadInProgress(int percent)
{
	setStyleSheet("QProgressBar::chunk{background-color: rgba(0, 0, 155, 0.2)}");
	setValue(percent);
}

void SUrlArea::loadFinished(bool ok)
{
	m_parent->getActions()->refreshOrStop->setIcon(QIcon(m_parent->getActions()->themePath + "refresh.png"));
	m_parent->getActions()->refreshOrStop->setText("Rafraichir la page");
	connect(m_parent->getActions()->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::refresh);
}

void SUrlArea::loadUrl()
{
	m_parent->currentPage()->load(QUrl(m_champs->text()));
}
