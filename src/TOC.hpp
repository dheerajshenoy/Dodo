#pragma once


class TreeWidget;

#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QVBoxLayout>

class TOC : public QWidget
{
public:
    TOC(QWidget *parent = nullptr);
    ~TOC();

    void fontSettings();

    inline TreeWidget* treeWidget() { return m_treeWidget; }

private:
    TreeWidget *m_treeWidget;
    QVBoxLayout *m_layout = new QVBoxLayout();

};

#include "TreeWidget.hpp"
