#pragma once

#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLineEdit>
#include <qt6/QtWidgets/QWidget>

#include "LineEdit.hpp"

class CommandBar : public QWidget
{

public:
    CommandBar(QWidget *parent = nullptr);
    ~CommandBar();

    LineEdit* lineEdit();
    void addWidget(QWidget *widget, int stretch = 0, Qt::AlignmentFlag align = Qt::AlignLeft);
    void handleCommand();
    void singlePart(QString);
    void multiPart(QStringList);

    void keyReleaseEvent(QKeyEvent*);

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    LineEdit *m_lineEdit = new LineEdit();

};
