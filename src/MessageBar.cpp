#include "MessageBar.hpp"

MessageBar::MessageBar(QWidget *parent)
    : QWidget(parent)
{
    this->setLayout(m_layout);
    m_layout->addWidget(m_label);
    this->hide();
}

MessageBar::~MessageBar()
{}

void MessageBar::showMessage(QString message, unsigned int ms)
{
    this->show();
    m_label->setText(message);

    QTimer::singleShot(ms, this, [this]() { this->hide(); });
    qDebug() << "DD";
}
