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

    QDate date{ QDate::currentDate() };
    QDate dateToShow{ date.year(), date.month(), date.day() - 7};

    for (date = QDate::currentDate(); date >= dateToShow; date = date.addDays(-1)) {
        SMainWindow::SSettings->beginGroup("History/" + QString::number(date.year()) + "/" + QString::number(date.month()) + "/" + QString::number(date.day()));

        if(SMainWindow::SSettings->value("itemNum", -1).toInt() == -1) {
            SMainWindow::SSettings->endGroup();
            continue;
        }

        for (int i{ SMainWindow::SSettings->value("itemNum", 0).toInt() - 1 }; i >= 0; --i) {
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
}

SHistoryWindow::~SHistoryWindow()
{
}
