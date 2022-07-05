#pragma once

#include <qt6/QtWidgets/QTreeWidget>
#include <qt6/QtWidgets/QTreeWidgetItem>
#include <qt6/QtWidgets/QHeaderView>
#include <qt6/QtGui/QKeyEvent>
#include <qt6/QtCore/QEvent>
#include <qt6/QtCore/QObject>

class TreeWidget : public QTreeWidget
{

public:
    TreeWidget(QWidget *parent = nullptr);
    ~TreeWidget();
    void nextItem();
    void prevItem();
    void expandCurrent();
    bool eventFilter(QObject *obj, QEvent *e);
    void handleDoubleClick(QTreeWidgetItem *item, int column);
    int calculateRow(const QModelIndex & index);
};
