#include "includes/SWidgets/SUrlArea.hpp"
#include "includes/SMainWindow.hpp"
#include "includes/SActions.hpp"

#include <QMessageBox>

SUrlArea::SUrlArea(SMainWindow * parent) :
	QProgressBar(parent),
	m_parent(parent)
{
	QHBoxLayout* layout{ new QHBoxLayout(this) };
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	m_champs->setStyleSheet("background: #00FFFFFF; border: none;");
	setStyleSheet("QProgressBar{ border: none; background-color: #FFFFFF; } QProgressBar::chunk{background-color: #FFFFFF;}");

	m_champs->setClearButtonEnabled(true);
	layout->addWidget(m_champs);

	setTextVisible(false);

	connect(m_champs, &QLineEdit::returnPressed, this, &SUrlArea::loadUrl);
}

SUrlArea::~SUrlArea()
{

}

void SUrlArea::setText(const QString& texte)
{
	m_champs->setText(texte);
}

void SUrlArea::loadStarted()
{
	// Change the refresh action to the stop action
	m_parent->getActions()->refreshOrStop->setIcon(QIcon(m_parent->getActions()->themePath + "stop.png"));
	m_parent->getActions()->refreshOrStop->setText(tr("Arrêter le chargement"));
	m_parent->getActions()->refreshOrStop->setShortcut(QKeySequence(""));
	connect(m_parent->getActions()->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::stop);

//	setStyleSheet("QProgressBar::chunk{background-color: rgba(0, 0, 155, 0.2); border: none;}");
	setStyleSheet("QProgressBar{ border: none; background-color: #FFFFFF; } QProgressBar::chunk{background-color: #000066FF; border-bottom: 3px solid #000000;}");

	QString url{ m_parent->currentPage()->url().toString() };
	if ((url.left(7) != "http://" && url.left(8) != "https://" && url.left(5) != "html/") && !url.isEmpty()) {
		QMessageBox warningMsgBox{ QMessageBox::Warning, tr("Site non sécurisé"), tr("Attention, le site sur lequel vous entrez n'est pas sécurisé !"), QMessageBox::Ignore | QMessageBox::Cancel, this };

		warningMsgBox.setButtonText(QMessageBox::Ignore, tr("Continuer"));
		warningMsgBox.setButtonText(QMessageBox::Cancel, tr("Retour a la sécurité"));

		if (warningMsgBox.exec() == QMessageBox::Cancel)
			m_parent->back();
	}
}

void SUrlArea::loadInProgress(int percent)
{
	setValue(percent);
}

void SUrlArea::loadFinished()
{
	SWebView* view{ static_cast<SWebView*>(sender()) };

	if (!view)
		return;

	m_parent->getActions()->refreshOrStop->setIcon(QIcon(m_parent->getActions()->themePath + "refresh.png"));
	m_parent->getActions()->refreshOrStop->setText(tr("Rafraîchir la page"));
	m_parent->getActions()->refreshOrStop->setShortcuts(QKeySequence::Refresh);
//	setStyleSheet("QProgressBar::chunk{background-color: rgba(200, 200, 200, 0.2); border: none;}");
	setStyleSheet("QProgressBar{ border: none; background-color: #FFFFFF; } QProgressBar::chunk{background-color: #FFFFFF;}");

	connect(m_parent->getActions()->refreshOrStop, &QAction::triggered, m_parent, &SMainWindow::refresh);

	if(!m_parent->privateBrowsing) {
		if (view->url().toString() == "about:blank")
			return;
		else if(SMainWindow::curSessionHistory.size() <= 0)
			SMainWindow::addHistoryItem(view->title(), view->url());
		else if(SMainWindow::getLastHistoryItem().title != view->title())
			SMainWindow::addHistoryItem(view->title(), view->url());
	}
}

void SUrlArea::loadUrl()
{
	QString url{};


//	QRegExp urlReg{ "^(https?\:\/\/)?([a-zA-Z0-9-]+\.)?([a-zA-Z0-9-]+\.)?[a-zA-Z0-9-]+\.[a-zA-Z]{2,4}(:[0-9]+)?(\/[a-zA-Z0-9-]*\/?|\/[a-zA-Z0-9]+\.[a-zA-Z0-9]{1,4})?$" };
	QRegExp urlReg{ R"regex((https?\:\/\/)?([a-zA-Z0-9-]+.)?([a-zA-Z0-9-]+.)?[a-zA-Z0-9-]+\.[a-zA-Z]{2,4}(:[0-9]+)?(\/[a-zA-Z0-9-]*\/?|\/[a-zA-Z0-9]+\.[a-zA-Z0-9]{1,4})?)regex" };

	if (!m_champs->text().contains(urlReg)) {
		url = "http://www.google.com/search?q=" + m_champs->text();
		url.replace(" ", "+");
	}
	else {
		if (m_champs->text().left(7) != "http://" && m_champs->text().left(8) != "https://")
			url = "https://" + m_champs->text();
		else
			url = m_champs->text();
	}

/*
	if (m_champs->text().left(7) != "http://" && m_champs->text().left(8) != "https://")
		url = "https://" + m_champs->text();
	else
		url = m_champs->text();
*/

	m_parent->currentPage()->load(QUrl(url));
}
