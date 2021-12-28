#include "TodoListModel.h"
#include "TodoList.h"

TodoListModel::TodoListModel(QObject *parent)
    : QAbstractListModel(parent)
    , mList(nullptr)
{
}

int TodoListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    // Safe gaurd against null pointer accesses
    if (parent.isValid() || !mList)
        return 0;

    return mList->items().size();
}

QVariant TodoListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mList)
        return QVariant();

    const TodoItem item = mList->items().at(index.row());

    switch (role) {
        case DoneRole:
            // QVariant class acts like a union for most common qt data types
            return QVariant(item.done);
        case DescriptionRole:
            return QVariant(item.description);
        case IdRole:
            return QVariant(item.id);
        case StatusRole:
            return QVariant(item.status);
    }

    return QVariant();
}

bool TodoListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!mList)
        return false;

    TodoItem item = mList->items().at(index.row());
    switch (role) {
        case DoneRole:
            // Now we convert back the QVariant by typecasting
            item.done = value.toBool();
            break;
        case DescriptionRole:
            item.description = value.toString();
            break;
        case IdRole:
            item.id = value.toUInt();
            break;
        case StatusRole:
            item.status = value.toUInt();
            break;
    }

    // Check whether data has changed
    if (mList->setItemAt(index.row(), item)) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags TodoListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> TodoListModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[DescriptionRole] = "description";
    names[IdRole] = "id";
    names[StatusRole] = "status";
    return names;
}

TodoList *TodoListModel::list() const
{
    return mList;
}

void TodoListModel::setList(TodoList *list)
{
    beginResetModel();

    if (mList)
        mList->disconnect(this); // Disconnet from the old list when assigning to a new list

    mList = list;

    if (mList) {
        connect(mList, &TodoList::preItemAppended, this, [=]() {
            const int index = mList->items().size();
            beginInsertRows(QModelIndex(), index, index);
        });

        connect(mList, &TodoList::postItemAppended, this, [=]() {
            endInsertRows();
        });

        connect(mList, &TodoList::preItemRemoved, this, [=](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });

        connect(mList, &TodoList::postItemRemoved, this, [=]() {
            endRemoveRows();
        });
    }

    endResetModel();
}