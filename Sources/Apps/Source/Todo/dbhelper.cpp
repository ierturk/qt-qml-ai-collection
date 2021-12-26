#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QtDebug>

#include "dbhelper.h"

DbHelper::DbHelper() {
    ConnectToDb();
}

void DbHelper::ConnectToDb()
{

    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }

    db.setDatabaseName(connectionString);

    if (!db.open()) {
        qDebug() << "Error: connection with database failed";
        return;
    
    } else {
       qDebug() << "Database: connection opened";
    }
 
    // Init
    QSqlQuery query(db);

    if (!db.tables().contains( QLatin1String("todo_list"))) {
        bool created = query.exec("CREATE TABLE IF NOT EXISTS todo_list ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                "barcode VARCHAR (320), "
                                "description VARCHAR (320), "
                                "isdeleted BOOLEAN DEFAULT (0), "
                                "done BOOLEAN DEFAULT (0), "
                                "addedtime TIMESTAMP DEFAULT NULL)");

        qDebug() << "Database: inititialized." << created;

        if(!query.exec("INSERT INTO todo_list(barcode, description) VALUES('80829010', 'Mentos CleanBreath')"))
        qWarning() << "MainWindow::DatabasePopulate - ERROR: " << query.lastError().text();
        if(!query.exec("INSERT INTO todo_list(barcode, description) VALUES('86918091', 'Parliament Night Blue')"))
        qWarning() << "MainWindow::DatabasePopulate - ERROR: " << query.lastError().text();
    }

}

DbHelper::~DbHelper()
{
    if (db.isOpen())
    {
        db.close();
        qDebug() << "Database: connection closed";
        removeDb(connectionName);
    }
}

bool DbHelper::isOpen() const
{
    return db.isOpen();
}

void DbHelper::removeDb(const QString &conName)
{
    QSqlDatabase::removeDatabase(conName);
    qDebug() << "Database: connection removed";
}



QSqlQueryModel* DbHelper::getRecords() const
{
    QSqlQueryModel* model = new QSqlQueryModel;
    QString q = "SELECT id, done, barcode, description FROM todo_list WHERE isdeleted = 0";
    model->setQuery(q);
    return model;
}

bool DbHelper::updateRecords(const TodoItem &item)
{
    bool success = false;

    if (!item.description.isEmpty())
    {
        QSqlQuery query(db);
        QString strQuery = "UPDATE todo_list SET done = :done, barcode = :barcode, description = :description, addedtime = datetime(CURRENT_TIMESTAMP, 'localtime') "
                           "WHERE id = :id";
        query.prepare(strQuery);
        query.bindValue(":done", QString::number(item.done));
        query.bindValue(":barcode", item.barcode);
        query.bindValue(":description", item.description);
        query.bindValue(":id", item.id);

        if (!query.exec())
        {
            qDebug() << query.lastError();
        }
        else
        {
            success = true;
            qDebug() << "Updated: id#" << item.id;
        }
    }
    else
    {
        qDebug() << "Update failed: description cannot be empty";
    }
    return success;
}

bool DbHelper::insertRecords(const TodoItem& item)
{
    bool success = false;

    if (!item.description.isEmpty())
    {
        QSqlQuery queryAdd;
        queryAdd.prepare("INSERT INTO todo_list(done, barcode, description, addedtime) "
                         "VALUES (:done, :barcode, :description, datetime(CURRENT_TIMESTAMP, 'localtime'))");
        queryAdd.bindValue(":done", item.done);
        queryAdd.bindValue(":barcode", item.barcode);
        queryAdd.bindValue(":description", item.description);

        if (queryAdd.exec())
        {
            success = true;
            qDebug() << "Inserted!";
        }
        else
        {
            qDebug() << "Insertion failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "Insertion failed: description cannot be empty";
    }

    QSqlQuery query(db);
    QString strQuery = "UPDATE todo_list SET isdeleted = 1, addedtime = datetime(CURRENT_TIMESTAMP, 'localtime') "
                       "WHERE id = :id";

    return success;
}

bool DbHelper::deleteRecordByID(const quint32 itemId)
{
    bool success = false;

    QSqlQuery query(db);
    QString strQuery = "UPDATE todo_list SET isdeleted = 1, addedtime = datetime(CURRENT_TIMESTAMP, 'localtime') "
                       "WHERE id = :id";
    query.prepare(strQuery);
    query.bindValue(":id", itemId);

    if (!query.exec())
    {
        qDebug() << query.lastError();
    }
    else
    {
        success = true;
        qDebug() << "Deleted: id#" << itemId;
    }

    return success;
}
