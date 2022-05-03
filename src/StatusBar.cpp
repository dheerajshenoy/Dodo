#include "StatusBar.hpp"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);
    m_layout->setContentsMargins(0, 0, 0, 0);
    this->setFixedHeight(m_statusBarHeight);
}

StatusBar::~StatusBar()
{}

void StatusBar::addWidget(QWidget *widget, Qt::AlignmentFlag align)
{
    m_layout->addWidget(widget, 0, align);
}

void StatusBar::addWidget(QWidget *widget, int stretch, Qt::AlignmentFlag align)
{
    m_layout->addWidget(widget, stretch, align);
}

void StatusBar::addWidget(QWidget *widget, QFont font, int stretch, Qt::AlignmentFlag align)
{
    widget->setFont(font);
    m_layout->addWidget(widget, stretch, align);
}

void StatusBar::addWidget(QWidget *widget, QString name, int px, int stretch, Qt::AlignmentFlag align)
{
    QFont font;
    font.setFamily(name);
    font.setPixelSize(px);
    widget->setFont(font);
    m_layout->addWidget(widget, stretch, align);
}

void StatusBar::toggle()
{
    if(this->isHidden())
        this->show();
    else
        this->hide();
}
