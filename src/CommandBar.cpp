#include "CommandBar.hpp"
#include "Dodo.hpp"

CommandBar::CommandBar(QWidget *parent)
    : QWidget(parent)
{
    m_layout->addWidget(m_lineEdit);
    m_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_layout);
    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this](){
        handleCommand();
        m_lineEdit->clearFocus();
        m_lineEdit->clear();
        this->hide();
    });

}

void CommandBar::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        lineEdit()->clearFocus();
        lineEdit()->clear();
        this->hide();
    }

    QWidget::keyPressEvent(event);
}

CommandBar::~CommandBar()
{}

inline void CommandBar::addWidget(QWidget *widget, int stretch, Qt::AlignmentFlag align)
{
    m_layout->addWidget(widget, stretch, align);
}

LineEdit* CommandBar::lineEdit()
{
    return m_lineEdit;
}

void CommandBar::handleCommand()
{
    QStringList text = m_lineEdit->text().split(":", Qt::SkipEmptyParts);
    QStringList cmd = text.at(0).split(" ");
    switch(cmd.size())
    {
    case 1:
        singlePart(cmd.at(0));
        break;

    case 2:
        multiPart(cmd);
        break;

    default:
        break;
    }
}

void CommandBar::singlePart(QString cmd)
{
    bool ok;
    int j = cmd.toInt(&ok);
    if(ok)
    {
        if(dodo->isPageValid(j))
        {
            dodo->setCurrentPage(j-1);
            dodo->renderPage();
        }
    }
    else
    {
        if (cmd == "outline")
        {
            dodo->toggleTOC();
        }
        else if (cmd == "open")
        {
            dodo->OpenFileDialog();
        }
        else if (cmd == "close" || cmd == "c")
        {
            dodo->CloseFile();
        }
        else if (cmd == "quit" || cmd == "q")
        {
            QApplication::quit();
        }
    }
}

void CommandBar::multiPart(QStringList cmd)
{
    QString operation = cmd.at(0).toLower();

    if (operation == "o" or operation == "open")
    {
        dodo->OpenFile(cmd.at(1));
        return;
    }
    else if (operation == "w" or operation == "write")
    {
        qDebug() << "WRITE";
        return;
    }

    else if (operation == "searchpage")
    {
        dodo->searchPage(cmd.at(1));
    }

    else if (operation == "search")
    {
        dodo->searchDocument(cmd.at(1));
    }

}
