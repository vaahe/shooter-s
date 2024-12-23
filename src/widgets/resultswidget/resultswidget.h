#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include "src/database/databasemanager.h"

#include <QList>
#include <QDateEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ResultsWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit ResultsWidget(QWidget* parent = nullptr);
    void displayResults(const QList<Result>& results);
    void getResults(const QString& userId);

private slots:
    void onSearchClicked();

private:
    void setupUI();

    QTableWidget* m_table;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QPushButton* m_searchButton;

    DatabaseManager* m_dbManager;
};

#endif // RESULTSWIDGET_H
