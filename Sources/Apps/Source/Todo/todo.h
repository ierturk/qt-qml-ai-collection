#ifndef TODO_H
#define TODO_H

#include <QObject>
#include <QVector>

struct TodoItem {
    bool done;
    QString barcode;
    QString description;
    quint32 id;
    quint8 status; // Default:0, Changed:1, New:2
};

class TodoList : public QObject
{
    Q_OBJECT
public:
    explicit TodoList(QObject *parent = nullptr);

    QVector<TodoItem> items() const;
    void getItems();
    bool setItemAt(int index, const TodoItem &item);

public slots:
    void appendItem();
    void removeCompletedItems();
//    void saveItems(const QString &taskDescr, const int &taskId, const bool &taskDone) const;
    void saveItems();

signals:
    void preItemAppended();
    void postItemAppended();

    void preItemRemoved(int index);
    void postItemRemoved();

private:
    QVector<TodoItem> mItems;

};

#endif // TODO_H
