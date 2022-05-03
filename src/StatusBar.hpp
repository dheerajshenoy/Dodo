#pragma once
#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QHBoxLayout>

#include <qt6/QtGui/QFont>

class StatusBar : public QWidget
{

public:
    StatusBar(QWidget *parent = nullptr);
    ~StatusBar();

    void addWidget(QWidget *widget, Qt::AlignmentFlag align = Qt::AlignLeft);
    void addWidget(QWidget *widget, int stretch, Qt::AlignmentFlag align = Qt::AlignLeft);
    void addWidget(QWidget *widget, QFont font, int stretch = 0, Qt::AlignmentFlag align = Qt::AlignLeft);
    void addWidget(QWidget *widget, QString font, int px = 14, int stretch = 0, Qt::AlignmentFlag align = Qt::AlignLeft);
    inline QHBoxLayout* Layout() { return m_layout; }
    void toggle();

private:
    QHBoxLayout *m_layout = new QHBoxLayout();

    unsigned int m_statusBarHeight = 25;
};
