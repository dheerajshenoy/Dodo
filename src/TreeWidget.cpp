#include "TreeWidget.hpp"

#include "Dodo.hpp"

TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    auto header = this->header();
    header->setStretchLastSection(false);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    this->headerItem()->setText(0, "Section");
    this->headerItem()->setText(1, "Page No.");

    this->installEventFilter(this);
    connect(this, &QTreeWidget::itemDoubleClicked, this, &TreeWidget::handleDoubleClick);
    connect(this, &QTreeWidget::itemActivated, this, &TreeWidget::handleDoubleClick);
}

TreeWidget::~TreeWidget()
{}

bool TreeWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == this && e->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
        switch(keyEvent->key())
        {
            case Qt::Key_H:
                collapseItem(currentItem());
                break;

            case Qt::Key_J:
                nextItem();
                break;

            case Qt::Key_K:
                prevItem();
                break;

            case Qt::Key_L:
                expandItem(currentItem());
                break;

        }
    }
    return false;
}

void TreeWidget::nextItem()
{
    QTreeWidgetItem *next = itemBelow(currentItem());
    if(!next) return;
    setCurrentItem(next);
}

void TreeWidget::prevItem()
{
    QTreeWidgetItem *prev = itemAbove(currentItem());
    if(!prev) return;
    setCurrentItem(prev);
}

void TreeWidget::expandCurrent()
{
    QTreeWidgetItem *item = currentItem();
    if(!item->isExpanded())
        this->expandItem(item);
    else
        this->collapseItem(item);
}

void TreeWidget::handleDoubleClick(QTreeWidgetItem *item, int column)
{
    int page = item->text(1).toInt();
    dodo->setCurrentPage(page - 1);
    dodo->pageScrollTop();
    dodo->m_stackedWidget->setCurrentIndex(0);
    dodo->renderPage();
}
