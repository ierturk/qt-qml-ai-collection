#ifndef TODOLISTMODEL_H
#define TODOLISTMODEL_H

#include <QAbstractListModel>

#include "TodoList.h"

class TodoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TodoList *list READ list WRITE setList)

public:
    explicit TodoListModel(QObject *parent = nullptr);
    /*
     * Roles are defined here
     */
    enum {
        DoneRole = Qt::UserRole,
        DescriptionRole,
        IdRole,
        StatusRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    /*
     * virtual means that the function will be overriden
     * const means if a const class created its members cant be changed
     * override means this function is overriding
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    TodoList *list() const;
    void setList(TodoList *list);

private:
    TodoList *mList;
};

#endif // TODOLISTMODEL_H