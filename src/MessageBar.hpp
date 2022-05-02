#pragma once

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLabel>

#include <qt6/QtCore/QTimer>

class MessageBar : public QWidget
{

public:
    MessageBar(QWidget *parent = nullptr);
    ~MessageBar();

    void showMessage(QString txt, unsigned int ms = 2500);

private:
    QHBoxLayout *m_layout = new QHBoxLayout();
    QLabel *m_label = new QLabel();

};
