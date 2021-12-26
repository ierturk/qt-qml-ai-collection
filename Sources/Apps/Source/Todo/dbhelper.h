#ifndef DBHELPER_H
#define DBHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQueryModel>

#include "todo.h"

static const QString connectionString = "/workspace/run/sqlite/db/todolist.db";
static const QString connectionName = "TodoListDBConnection";

class DbHelper : public QObject
{
    Q_OBJECT

public:
    DbHelper();
    ~DbHelper();
    void ConnectToDb();
    bool isOpen() const;
    void removeDb(const QString &conName);

    /**
     * @brief Select items from db
     * @param no any parameters
     * @return QSqlQueryModel with all records
     */
    QSqlQueryModel* getRecords() const;

    /**
     * @brief Updates items to db
     * @param item - contains description, task id of changed item
     * @return true - task was successfully updated, false - task not updated
     */
    bool updateRecords(const TodoItem& item);

    /**
     * @brief Inserts new items to db
     * @param item - tasks that needs to be inserted
     * @return true - task was inserted successfully, false - task not inserted
     */
    bool insertRecords(const TodoItem& item);

    /**
     * @brief Deletes item by ID through updating required column
     * @param item - tasks that needs to be deleted
     * @return true - task was deleted successfully, false - task not deleted
     */
    bool deleteRecordByID(const quint32 itemId);

private:
    QSqlDatabase db;

};

#endif // DBHELPER_H
