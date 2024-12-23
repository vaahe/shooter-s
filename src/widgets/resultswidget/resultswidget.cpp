#include "resultswidget.h"

ResultsWidget::ResultsWidget(QWidget* parent) :
    QTableWidget(parent),
    m_table(new QTableWidget(this)),
    m_startDateEdit(new QDateEdit(this)),
    m_endDateEdit(new QDateEdit(this)),
    m_searchButton(new QPushButton("Search", this)),
    m_dbManager(DatabaseManager::getInstance())
{
    setupUI();

    getResults("1");
}

void ResultsWidget::setupUI()
{
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");

    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");

    m_table->setColumnCount(5);
    QStringList headers = {"Result", "User ID", "Distance", "Imitation Distance", "Date"};
    m_table->setHorizontalHeaderLabels(headers);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (int i = 0; i < m_table->columnCount(); ++i) {
        m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLayout->addWidget(m_startDateEdit);
    searchLayout->addWidget(m_endDateEdit);
    searchLayout->addWidget(m_searchButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(m_table);

    setLayout(mainLayout);
    setMinimumSize(700, 400);

    connect(m_searchButton, &QPushButton::clicked, this, &ResultsWidget::onSearchClicked);
}

void ResultsWidget::onSearchClicked()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();

    if (startDate.isNull() || endDate.isNull() || startDate > endDate) {
        qWarning() << "Invalid date range selected.";
        return;
    }

    QString userId = "1";
    QList<Result> results = m_dbManager->getResultsByRange(userId, startDate, endDate);

    displayResults(results);
}

void ResultsWidget::displayResults(const QList<Result>& results)
{
    m_table->setRowCount(results.size());

    for (int row = 0; row < results.size(); ++row) {
        const Result& result = results.at(row);

        QTableWidgetItem* resultItem = new QTableWidgetItem(result.result);
        QTableWidgetItem* userIdItem = new QTableWidgetItem(result.userId);
        QTableWidgetItem* distanceItem = new QTableWidgetItem(QString::number(result.distance));
        QTableWidgetItem* imitationDistanceItem = new QTableWidgetItem(QString::number(result.imitationDistance));

        const QStringList formattedDate = result.date.split('T');
        QTableWidgetItem* dateItem = new QTableWidgetItem(formattedDate.join(' '));

        m_table->setItem(row, 0, resultItem);
        m_table->setItem(row, 1, userIdItem);
        m_table->setItem(row, 2, distanceItem);
        m_table->setItem(row, 3, imitationDistanceItem);
        m_table->setItem(row, 4, dateItem);
    }
}

void ResultsWidget::getResults(const QString &userId) {
    QList<Result> results = m_dbManager->getResults(userId);

    displayResults(results);
}
