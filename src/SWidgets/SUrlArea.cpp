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

    setStyleSheet("QProgressBar::chunk{background-color: rgba(0, 0, 155, 0.2)}");

    QString url{ m_parent->currentPage()->url().toString() };
    if ((url.left(7) != "http://" && url.left(8) != "https://" && url.left(5) != "html/") && !url.isEmpty()) {
        QMessageBox warningMsgBox{ QMessageBox::Warning, tr("Site non sécurisé"), tr("Attention, le site sur lequel vous entrez n'est pas sécurisé !"), QMessageBox::Ignore | QMessageBox::Cancel, this };

        warningMsgBox.setButtonText(QMessageBox::Ignore, tr("Continuer"));
        warningMsgBox.setButtonText(QMessageBox::Cancel, tr("Retour à la sécurité"));

        if (warningMsgBox.exec() == QMessageBox::Cancel)
            m_parent->back();
    }
}

void SUrlArea::loadInProgress(int percent)
{
    setValue(percent);
}

void SUrlArea::loadFinished(bool ok)
{
    SWebView* view{ static_cast<SWebView*>(sender()) };

    if (!view)
        return;

    m_parent->getActions()->refreshOrStop->setIcon(QIcon(m_parent->getActions()->themePath + "refresh.png"));
    m_parent->getActions()->refreshOrStop->setText("Rafraichir la page");
    setStyleSheet("QProgressBar::chunk{background-color: rgba(200, 200, 200, 0.2)}");
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
    if (m_champs->text().left(7) != "http://" && m_champs->text().left(8) != "https://")
        url = "https://" + m_champs->text();
    else
        url = m_champs->text();

    m_parent->currentPage()->load(QUrl(url));
}
