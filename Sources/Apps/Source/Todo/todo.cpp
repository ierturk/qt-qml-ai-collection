#include <QDebug>
#include "todo.h"
#include "dbhelper.h"
#include <QSqlRecord>
#include <QSqlField>

TodoList::TodoList(QObject *parent) : QObject(parent)
{
    //Dinamically populates from the database

    getItems();

    // adding some initial sample data
    // mItems.append({true, QStringLiteral("Attend a meeting at 1:45pm")});
    // mItems.append({false, QStringLiteral("Fix the sink")});
}

QVector<TodoItem> TodoList::items() const
{
    return mItems;
}

void TodoList::getItems()
{
    DbHelper dbh;
    QSqlQueryModel* getRecords = dbh.getRecords();
    int rowCount = getRecords->rowCount();
    if (rowCount > 0)
    {
        mItems.clear();
        for(int i = 0; i < rowCount; ++i)
        {
            uint id = getRecords->record(i).value("id").toUInt();
            bool done = getRecords->record(i).value("done").toBool();
            QString barcode = getRecords->record(i).value("barcode").toString();
            QString descr = getRecords->record(i).value("description").toString();
            mItems.append({ done, barcode, descr, id, 0 });
        }
    }
    else
    {
        qDebug() << "Empty table";
    }
}

bool TodoList::setItemAt(int index, const TodoItem &item)
{
    /*
     * check if the index is valid
     */
    if (index < 0 || index >= mItems.size())
        return false;

    const TodoItem &oldItem = mItems.at(index);
    /*
     * checks if new item has identical
     * values or not to check whether we are changing
     * the data or not
     */

    if (item.done == oldItem.done && item.description == oldItem.description && item.status == oldItem.status)
        return false;

    mItems[index] = item;
    return true;
}

void TodoList::appendItem()
{
    emit preItemAppended();

    TodoItem item;
    item.done = false;
    item.id = 0; //Temporary id
    item.status = 2;
    mItems.append(item);

    emit postItemAppended();
}

//Check on all items and remove the ones that are done
void TodoList::removeCompletedItems()
{
    DbHelper dbh;

    for (int i = 0; i < mItems.size();)
    {
        if (mItems.at(i).done)
        {
            qDebug() << i;
            emit preItemRemoved(i); //Emitting the signal

            dbh.deleteRecordByID(mItems.at(i).id);
            mItems.removeAt(i);

            emit postItemRemoved(); //Emitting the signal
        }
        else
        {
            ++i;
        }
    }

}

void TodoList::saveItems()
{
    DbHelper dbh;
    for (TodoItem &item: mItems)
    {
        if (item.status == 1) //Update
        {
            dbh.updateRecords(item);
        }
        else if (item.status == 2) //Insert
        {
            dbh.insertRecords(item);
        }
        item.status = 0;
    }
    getItems(); //Render
}
