#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include "src/database/databasemanager.h"
#include "src/utilities/globalsmanager.h"

#include <QList>
#include <QDateEdit>
#include <QPushButton>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTableWidget>


class ResultsWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit ResultsWidget(QWidget* parent = nullptr);
    void displayResults(const QList<Result>& results);
    void getResults();
    void retranslateUI();


private slots:
    void setupUI();
    void onSearchClicked();


private:
    QTableWidget* m_table;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QPushButton* m_searchButton;
    DatabaseManager* m_dbManager;
    GlobalsManager &m_globalsManager = GlobalsManager::getInstance();
};

#endif // RESULTSWIDGET_H
