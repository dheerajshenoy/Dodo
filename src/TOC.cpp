
#include "TOC.hpp"

TOC::TOC(QWidget *parent)
    : QWidget(parent)
{
    m_treeWidget = new TreeWidget();
    this->setLayout(m_layout);
    m_layout->addWidget(m_treeWidget, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    fontSettings();
}

void TOC::fontSettings()
{
    QFont font;
    font.setFamily("Rajdhani Semibold");
    font.setPixelSize(20);
    m_treeWidget->setFont(font);
}

TOC::~TOC()
{}
