#include "includes/SWindows/SHistory.hpp"
#include "includes/SMainWindow.hpp"

SHistoryWindow::SHistoryWindow(SMainWindow * parent) :
	QDialog(parent),
	m_parent(parent)
{
	setWindowTitle("Historique de navigation");
	resize(758, 450);

	m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_view->setProperty("showDropIndicator", QVariant(false));
	m_view->setAlternatingRowColors(true);
	m_view->setAnimated(true);
	m_view->header()->setDefaultSectionSize(300);
	m_view->setModel(m_model);

	m_boxBtn->setStandardButtons(QDialogButtonBox::Ok);

	m_btnLayout->addWidget(m_loadBtn);
	m_btnLayout->addWidget(m_deleteAllBtn);
	m_btnLayout->addItem(m_hSpacer);
	m_btnLayout->addWidget(m_boxBtn);

	m_layout->addWidget(m_view);
	m_layout->addLayout(m_btnLayout);

	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SHistoryWindow::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SHistoryWindow::reject);

	QList<QStandardItem*> historyItemTitle{};
	QList<QStandardItem*> historyItemUrl{};

    for(int i{ m_parent->getCurSessionHistory().size() - 1 }; i >= 0; --i) {
        historyItemTitle.append(new QStandardItem(m_parent->getCurSessionHistory()[i].title));
        historyItemUrl.append(new QStandardItem(m_parent->getCurSessionHistory()[i].url.toString()));
    }
    QStandardItem *currentSessionItem{ new QStandardItem("Session actuelle") };
    currentSessionItem->appendColumn(historyItemTitle);
    currentSessionItem->appendColumn(historyItemUrl);
    m_model->appendRow(currentSessionItem);

    m_view->expandAll();

    historyItemTitle.clear();
    historyItemUrl.clear();

    QDate date{ QDate::currentDate() };
    QDate dateToShow{ QDate::currentDate().addDays(-7) };

    for (date = QDate::currentDate(); date >= dateToShow; date = date.addDays(-1)) {
        SMainWindow::SSettings->beginGroup("History/" + QString::number(date.year()) + "/" + QString::number(date.month()) + "/" + QString::number(date.day()));

        if(!SMainWindow::SSettings->contains("itemNum")) {
            SMainWindow::SSettings->endGroup();
            continue;
        }

        for (int i{ SMainWindow::SSettings->value("itemNum", 0).toInt() - 1 }; i >= 0; --i) {
            if(!SMainWindow::SSettings->contains(QString::number(i) + "/title")) {
                SMainWindow::SSettings->endGroup();
                continue;
            }

            QString title{ SMainWindow::SSettings->value(QString::number(i) + "/title", "nullTitle").toString() };
            QString url{ SMainWindow::SSettings->value(QString::number(i) + "/url", "nullUrl").toString() };

            historyItemTitle.append(new QStandardItem(title));
            historyItemUrl.append(new QStandardItem(url));

        }
        SMainWindow::SSettings->endGroup();

        QStandardItem *dateItem{ new QStandardItem(QString::number(date.month()) + "/" + QString::number(date.day())) };

        if(date == QDate::currentDate())
            dateItem->setText("Aujourd'hui");
        if(date == QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day() - 1))
            dateItem->setText("Hier");

        dateItem->insertColumn(0, historyItemTitle);
        dateItem->insertColumn(1, historyItemUrl);

        historyItemTitle.clear();
        historyItemUrl.clear();

        m_model->appendRow(dateItem);

    }


	QStringList headersNams;
	headersNams.push_back("Nom des pages");
	headersNams.push_back("Adresses");
	m_model->setHorizontalHeaderLabels(headersNams);

    connect(m_view, &QTreeView::doubleClicked, this, &SHistoryWindow::load);
    connect(m_loadBtn, &QPushButton::clicked, this, &SHistoryWindow::load);
    connect(m_deleteAllBtn, &QPushButton::clicked, this, &SHistoryWindow::deleteAll);
}

SHistoryWindow::~SHistoryWindow()
{
}

void SHistoryWindow::load()
{
    QModelIndex index{ m_view->currentIndex() };
    QString title{};
    QUrl url{};

    if(m_model->itemFromIndex(index)->parent() == nullptr)
        return;

    if(index.column() == 1) {
        title = m_model->data(m_model->itemFromIndex(index)->parent()->child(index.row())->index()).toString();
        url = m_model->data(index).toUrl();
    }
    else {
        title = m_model->data(index).toString();
        url = m_model->data(m_model->itemFromIndex(index)->parent()->child(index.row(), 1)->index()).toUrl();
    }

    m_parent->getTabs()->createWebTab(title, url);
    m_parent->getTabs()->createPlusTab();
    m_parent->getTabs()->removeTab(m_parent->getTabs()->count() - 3);
}

void SHistoryWindow::deleteAll()
{
    m_parent->getCurSessionHistory().clear();
    SMainWindow::SSettings->remove("History");

    m_model->clear();
}
