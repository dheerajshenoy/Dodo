#pragma once

#include <QKeyEvent>
#include <QLineEdit>
#include <QKeySequence>
#include <QShortcut>

class LineEdit : public QLineEdit
{
    public:
        LineEdit(QWidget *parent = nullptr);
        ~LineEdit();

};
