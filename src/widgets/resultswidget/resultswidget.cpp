#include "resultswidget.h"


ResultsWidget::ResultsWidget(QWidget* parent) :
    QTableWidget(parent),
    m_table(new QTableWidget(this)),
    m_startDateEdit(new QDateEdit(this)),
    m_endDateEdit(new QDateEdit(this)),
    m_searchButton(new QPushButton(tr("Search"), this)),
    m_dbManager(DatabaseManager::getInstance())
{
    setupUI();
    retranslateUI();
    setWindowTitle(tr("Shooter S: Results"));
}


void ResultsWidget::setupUI()
{
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_startDateEdit->setDate(QDate::currentDate().addMonths(-1));

    m_endDateEdit->setCalendarPopup(true);
    m_endDateEdit->setDisplayFormat("yyyy-MM-dd");
    m_endDateEdit->setDate(QDate::currentDate());

    m_table->setColumnCount(5);
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
    getResults();

    connect(m_searchButton, &QPushButton::clicked, this, &ResultsWidget::onSearchClicked);
}


void ResultsWidget::onSearchClicked()
{
    QDate startDate = m_startDateEdit->date();
    QDate endDate = m_endDateEdit->date();

    if (startDate.isNull() || endDate.isNull() || startDate > endDate) {
        QMessageBox::warning(this, tr("Invalid Date Range"), tr("Please ensure that both start and end dates are valid."));
        return;
    }

    QString userId = m_globalsManager.getUserId();
    QList<Result> results = m_dbManager->getResultsByRange(userId, startDate, endDate);

    displayResults(results);
}


void ResultsWidget::displayResults(const QList<Result>& results)
{
    if (results.size() == 0) {
        m_table->setRowCount(1);

        QTableWidgetItem *noDataItem = new QTableWidgetItem(tr("No data found"));
        noDataItem->setTextAlignment(Qt::AlignCenter);
        m_table->setItem(0, 0, noDataItem);
        m_table->setSpan(0, 0, 1, m_table->columnCount());

        return;
    }

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


void ResultsWidget::getResults() {
    const QString userId = m_globalsManager.getUserId();
    QList<Result> results = m_dbManager->getResults(userId);

    displayResults(results);
}


void ResultsWidget::retranslateUI() {
    setWindowTitle(tr("Shooter S: Results"));
    m_searchButton->setText(tr("Search"));

    QStringList headers = {tr("Result"), tr("User ID"), tr("Distance"), tr("Imitation Distance"), tr("Date")};
    m_table->setHorizontalHeaderLabels(headers);
}
