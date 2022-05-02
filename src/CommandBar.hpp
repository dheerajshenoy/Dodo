#pragma once

#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLineEdit>
#include <qt6/QtWidgets/QWidget>

class CommandBar : public QWidget
{

public:
    CommandBar(QWidget *parent = nullptr);
    ~CommandBar();

    QLineEdit* lineEdit();
    void addWidget(QWidget *widget, int stretch = 0, Qt::AlignmentFlag align = Qt::AlignLeft);
    void handleCommand();
    void singlePart(QString);
    void multiPart(QStringList);


private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLineEdit *m_lineEdit = new QLineEdit();

};
