#include "Dodo.hpp"
#include "qevent.h"
#include "qnamespace.h"
#include <memory>
#include <poppler/qt6/poppler-link.h>
#include <qt6/QtGui/qshortcut.h>
Dodo *dodo = nullptr;

#include "TOC.hpp"
#include "StatusBar.hpp"
#include "CommandBar.hpp"

int Dodo::m_pageNumber = 0;

Dodo::Dodo(int argc, char **argv)
    :m_argc(std::move(argc)), m_argv(std::move(argv))
{
    $HOME = getenv("HOME");
    m_layout->setSpacing(0);
    m_widget->setLayout(m_layout);
    m_layout->addWidget(m_stackedWidget);
    m_stackedWidget->addWidget(m_mainWidget);
    m_mainWidget->setLayout(m_mainLayout);

    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollWidgetLayout->setContentsMargins(0, 0, 0, 0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_mainLayout->addWidget(m_scrollArea);
    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollWidget->setLayout(m_scrollWidgetLayout);
    m_scrollArea->setWidgetResizable(true);
    m_scrollWidget->setBackgroundRole(QPalette::Dark);
    m_scrollWidgetLayout->addWidget(m_img, 0, Qt::AlignCenter);
    this->setCentralWidget(m_widget);
    pageSettings();
    Init();
    toggleVHScrollBars();
    ZoomReset();

}

Dodo::~Dodo()
{
}

void Dodo::pageSettings()
{
    m_img->setScaledContents(true);
}

void Dodo::Init()
{
    connect(m_VScrollBar, &QAbstractSlider::valueChanged, this, &Dodo::handleVscroll);
    connect(this, SIGNAL( currentPageChanged(int) ), this, SLOT( pageChanged(int) ));
    connect(this, SIGNAL( documentChanged(Poppler::Document*) ), this,
            SLOT( handleDocumentChanged(Poppler::Document*) ));
    handleKeys();
    InitTOC();
    InitStatusBar();
    InitCommandBar();
    if(m_argc > 1)
    {
        for(int i=1; i < m_argc; i++)
            OpenFile(m_argv[i]);
    }
}

void Dodo::OpenFile(QString fileName)
{
    QString file = fileName.replace("~", $HOME);

    if(!QFile::exists(file))
    {
        m_messageBar->showMessage("Unable to find the file");
        return;
    }

    m_doc = Poppler::Document::load(file);
    if(!m_doc)
    {
        m_messageBar->showMessage("Unable to load the file");
        return;
    }
    m_doc->setRenderHint(Poppler::Document::TextAntialiasing);
    m_doc->setRenderHint(Poppler::Document::Antialiasing);
    emit(documentChanged(m_doc.get()));
    m_docNameLabel->setText(fileName.replace($HOME, "~"));
    processOutline();
    m_pCount = m_doc->numPages();
    m_page = m_doc->page(m_pageNumber);
	gotoPage(0);
    ZoomReset();
}

void Dodo::OpenFileDialog()
{
    QString filename = QFileDialog::getOpenFileName();

    if(!QFile::exists(filename))
    {
        m_messageBar->showMessage("Unable to find the file");
        return;
    }

    m_doc = Poppler::Document::load(filename);
    if(!m_doc)
    {
        m_messageBar->showMessage("Unable to load the file");
        return;
    }
    m_doc->setRenderHint(Poppler::Document::TextAntialiasing);
    m_doc->setRenderHint(Poppler::Document::Antialiasing);
    emit(documentChanged(m_doc.get()));
    m_docNameLabel->setText(filename.replace($HOME, "~"));
    processOutline();
    m_pCount = m_doc->numPages();
    m_page = m_doc->page(m_pageNumber);
	gotoPage(0);
    ZoomReset();
}

void Dodo::InitStatusBar()
{
    m_statusBar = new StatusBar();
    m_statusBar->Layout()->setContentsMargins(0, 0, 0, 0);
    m_statusBar->Layout()->addStretch(1);
    m_statusBar->addWidget(m_docNameLabel, Qt::AlignVCenter);
    m_statusBar->Layout()->addStretch(100);
    m_statusBar->addWidget(m_pageNumberLabel);
    m_statusBar->addWidget(m_pageCountDividerLabel);
    m_statusBar->addWidget(m_pageCountLabel);
    m_statusBar->Layout()->addStretch(1);
    m_layout->addWidget(m_statusBar);
    //currentPageChanged();
}

void Dodo::handleDocumentChanged(Poppler::Document *doc)
{
    m_pageCountLabel->setText(QString::number(doc->numPages()));
}

void Dodo::InitCommandBar()
{
    m_commandBar = new CommandBar();
    m_layout->addWidget(m_commandBar);
    m_commandBar->hide();
    m_layout->addWidget(m_messageBar);
}

void Dodo::ZoomReset()
{
    m_zoomFactor = 1;
    scaleImage(0.35);
}

void Dodo::ZoomIn()
{
    scaleImage(1.25);
}

void Dodo::ZoomOut()
{
    if(m_zoomFactor > 0.02)
        scaleImage(0.8);
}

void Dodo::scaleImage(double factor)
{
    m_zoomFactor *= factor;
	m_imageScale = m_zoomFactor * m_img->pixmap(Qt::ReturnByValue).size();
    m_img->setFixedSize(m_imageScale);
}

void Dodo::handleKeys()
{
    QShortcut *zoomIn = new QShortcut(QKeySequence("="), this);
    QShortcut *zoomOut = new QShortcut(QKeySequence("-"), this);
    QShortcut *scrollUp = new QShortcut(QKeySequence("k"), this);
    QShortcut *scrollDown = new QShortcut(QKeySequence("j"), this);
    QShortcut *scrollLeft = new QShortcut(QKeySequence("h"), this);
    QShortcut *scrollRight = new QShortcut(QKeySequence("l"), this);
    QShortcut *toggleScrollBars = new QShortcut(QKeySequence("s"), this);
    QShortcut *toggleStatusBar = new QShortcut(QKeySequence("x"), this);
    QShortcut *command = new QShortcut(QKeySequence(":"), this);
    QShortcut *gotoBeg = new QShortcut(QKeySequence("g,g"), this);
    QShortcut *gotoEnd = new QShortcut(QKeySequence("Shift+G"), this);
    QShortcut *togTOC = new QShortcut(QKeySequence("Tab"), this);
    QShortcut *darkMode = new QShortcut(QKeySequence("i"), this);
    QShortcut *invert = new QShortcut(QKeySequence("q"), this);
    QShortcut *k_fit_to_width = new QShortcut(QKeySequence("W"), this);
	QShortcut *k_escape = new QShortcut(QKeySequence("Escape"), this);

    connect(invert, &QShortcut::activated, this, &Dodo::toggleRecolor);
    connect(zoomIn, &QShortcut::activated, this, &Dodo::ZoomIn);
    connect(zoomOut, &QShortcut::activated, this, &Dodo::ZoomOut);
    connect(scrollUp, &QShortcut::activated, this, [this](){ Dodo::Scroll(Up); });
    connect(scrollDown, &QShortcut::activated, this, [this](){ Dodo::Scroll(Down); });
    connect(scrollLeft, &QShortcut::activated, this, [this](){ Dodo::Scroll(Left); });
    connect(scrollRight, &QShortcut::activated, this, [this](){ Dodo::Scroll(Right); });
    connect(toggleScrollBars, &QShortcut::activated, this, &Dodo::toggleVHScrollBars);
    connect(toggleStatusBar, &QShortcut::activated, this, [this]() { m_statusBar->toggle(); });
    connect(gotoBeg, &QShortcut::activated, this, &Dodo::begOfDocument);
    connect(gotoEnd, &QShortcut::activated, this, &Dodo::endOfDocument);
    connect(togTOC, &QShortcut::activated, this, &Dodo::toggleTOC);
    connect(darkMode, &QShortcut::activated, this, &Dodo::toggleDarkMode);
    connect(command, &QShortcut::activated, this, [this]() {
            if(m_commandBar->isHidden())
            {
                m_commandBar->show();
                m_commandBar->lineEdit()->setFocus();
                m_commandBar->lineEdit()->setText(":");
            }
            else
                m_commandBar->hide();
            });
    connect(k_fit_to_width, &QShortcut::activated, this, &Dodo::FitToWidth);
    // connect(k_escape, &QShortcut::activated, this, &Dodo::Escape);

}

void Dodo::gotoPage(int page)
{
	setCurrentPage(page);
	renderPage();
}

void Dodo::setCurrentPage(int page)
{
    if(!isPageValid(page)) return;
    m_pageNumber = page;
    emit currentPageChanged();
}

void Dodo::renderPage(int page)
{
    if (isAlreadyCached(page))
    {
        m_img->setPixmap(m_img_map.value(page));
        getLinks();
        return;
    }
    m_page = m_doc->page(page);
    m_currentImage = m_page->renderToImage(m_DPI, m_DPI);
    if(m_recolor)
    {
        m_currentImage.invertPixels(QImage::InvertRgb);
        for(int x=0; x < m_currentImage.width(); x++)
            for(int y=0; y < m_currentImage.height(); y++)
            {
                tmpColor = m_currentImage.pixelColor(x, y);
                if(tmpColor == QColor::fromRgb(0, 0, 0))
                    tmpColor.fromString("#370053");
                else if(tmpColor == QColor::fromRgb(255, 255, 255))
                    tmpColor.fromString("#FFFFFF");
                m_currentImage.setPixelColor(x, y, tmpColor);
            }
    }
    else
    {
        if(m_darkMode)
            m_currentImage.invertPixels(QImage::InvertRgb);
    }
    m_img->setPixmap(QPixmap::fromImage(m_currentImage));
    getLinks();
}

void Dodo::cachePage(int page)
{
    if(isAlreadyCached(page))
        return;
    m_page = m_doc->page(page);
    m_currentImage = m_page->renderToImage(m_DPI, m_DPI);
    if(m_recolor)
    {
        m_currentImage.invertPixels(QImage::InvertRgb);
        for(int x=0; x < m_currentImage.width(); x++)
            for(int y=0; y < m_currentImage.height(); y++)
            {
                tmpColor = m_currentImage.pixelColor(x, y);
                if(tmpColor == QColor::fromRgb(0, 0, 0))
                    tmpColor.fromString("#370053");
                else if(tmpColor == QColor::fromRgb(255, 255, 255))
                    tmpColor.fromString("#FFFFFF");

                m_currentImage.setPixelColor(x, y, tmpColor);
            }
    }
    else
    {
        if(m_darkMode)
            m_currentImage.invertPixels(QImage::InvertRgb);
    }
    m_img_map.insert(page, QPixmap::fromImage(m_currentImage));
    getLinks();
}

inline bool Dodo::isAlreadyCached(int page)
{
    return m_img_map.contains(page);
}

void Dodo::removeCachedPage(int page)
{
    m_img_map.remove(page);
}

bool Dodo::nextPage()
{
    if (m_pageNumber < m_pCount - 1)
    {
        setCurrentPage(getCurrentPage() + 1);
        emit currentPageChanged();
        
        return true;
    }
    return false;
}

bool Dodo::prevPage()
{
    if (m_pageNumber > 0)
    {
        setCurrentPage(getCurrentPage() - 1);
        emit currentPageChanged();
        return true;
    }
    return false;
}

void Dodo::Scroll(Direction direction)
{
    switch(direction)
    {
        case Up:
            if(m_VScrollBar->value() != m_VScrollBar->minimum())
                m_VScrollBar->setValue(m_VScrollBar->value() - m_xScrollAmount);
            else
            {
                if(prevPage())
                {
                    pageScrollBottom();
                    renderPage();
                }
            }
            break;

        case Down:
            if(m_VScrollBar->value() != m_VScrollBar->maximum())
                m_VScrollBar->setValue(m_VScrollBar->value() + m_xScrollAmount);
            else
            {
                if(nextPage())
                {
                    pageScrollTop();
                    renderPage();
                }
            }
            break;

        case Left:
            m_HScrollBar->setValue(m_HScrollBar->value() - m_yScrollAmount);
            break;

        case Right:
            m_HScrollBar->setValue(m_HScrollBar->value() + m_yScrollAmount);
            break;

    }
}

void Dodo::FitToWidth()
{
    //m_img->setFixedSize(m_zoomFactor);
    //renderPage();
}

void Dodo::FitToHeight()
{

}

void Dodo::FitToWindow()
{

}

void Dodo::pageChanged(int page)
{
    m_pageNumberLabel->setText(QString::number(page + 1));
}

void Dodo::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_J:
            if(event->modifiers() & Qt::ShiftModifier)
                nextPage();
            break;

        case Qt::Key_K:
            if(event->modifiers() & Qt::ShiftModifier)
                prevPage();
            break;
    }
}

void Dodo::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_J:
        case Qt::Key_K:
            if(event->modifiers() & Qt::ShiftModifier)
                if(!event->isAutoRepeat())
                    renderPage();
            break;
    }
}

void Dodo::toggleVHScrollBars()
{
    m_VScrollBar->setStyleSheet("QScrollBar { width: 0px; }");
    m_HScrollBar->setStyleSheet("QScrollBar { height: 0px; }");
}

void Dodo::begOfDocument()
{
    pageScrollTop();
	gotoPage(0);
}

void Dodo::endOfDocument()
{
    pageScrollTop();
	gotoPage(m_pCount - 1);
}

void Dodo::toggleTOC()
{
    if(m_stackedWidget->currentWidget() == m_mainWidget)
    {
        m_stackedWidget->setCurrentWidget(m_tocWidget);
        m_tocWidget->treeWidget()->setFocus();
    }
    else
    {
        m_stackedWidget->setCurrentWidget(m_mainWidget);
        m_tocWidget->treeWidget()->clearFocus();
    }
}

void Dodo::InitTOC()
{
    m_tocWidget = new TOC();
    m_stackedWidget->addWidget(m_tocWidget);
}

void Dodo::processOutline()
{
    m_outlines = m_doc->outline();

    /*
       qDebug() << "\n";
       for(auto &i : m_outlines)
       {
       auto d = i.destination();
       qDebug() << i.name() << " " << d.get()->pageNumber();
       if(i.hasChildren())
       {
       qDebug() << "\n";
       auto items = i.children();
       for(auto &item : items)
       {
       auto e = item.destination();
       qDebug() << item.name() << " " << e.get()->pageNumber();

       }
       qDebug() << "\n";
       }
       }
       */

    m_tocWidget->treeWidget()->clear();
    m_outlineSize = m_outlines.size();
    if(m_outlines.size() == 0) return;

    for(auto &outline : m_outlines)
    {
        // heading
        auto dest = outline.destination();
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem(m_tocWidget->treeWidget());
        treeWidgetItem->setText(0, outline.name());
        treeWidgetItem->setText(1, QString::number(dest.get()->pageNumber()));

        // sub-headings
        if(outline.hasChildren())
        {
            auto sub_children = outline.children();
            for(auto &sub_child : sub_children)
            {
                auto sub_dest = sub_child.destination();
                QTreeWidgetItem *childItem = new QTreeWidgetItem();
                childItem->setText(0, sub_child.name());
                childItem->setText(1, QString::number(sub_dest.get()->pageNumber()));
                treeWidgetItem->addChild(childItem);
                // sub headings
                m_outlineSize++;
            }
        }
        m_tocWidget->treeWidget()->addTopLevelItem(treeWidgetItem);
    }
}

void Dodo::toggleDarkMode()
{
    m_darkMode = !m_darkMode;
    renderPage();
}

void Dodo::handleVscroll(int value)
{

}

bool Dodo::isPageValid(int page)
{
    if(page < m_pCount && page > -1)
        return true;
    return false;
}

void Dodo::CloseFile()
{
    m_img->clear();
    m_pageNumberLabel->setText("");
    m_docNameLabel->setText("");
}

void Dodo::invertToColor()
{}

void Dodo::toggleRecolor()
{
    m_recolor = !m_recolor;
}

// TODO: Get all the link in the page
void Dodo::getLinks()
{}

void Dodo::pageScrollTop()
{
    m_VScrollBar->setValue(0);
}

void Dodo::pageScrollBottom()
{
    m_VScrollBar->setValue(m_VScrollBar->maximum());
}

void Dodo::searchPage(const QString str, const int p)
{
	auto page= m_doc->page(p);
	QList<QRectF> highlightRect = page->search(str);
	QPainter qImagePaint(&m_currentImage);

	qImagePaint.setBrush(Qt::NoBrush);
	qImagePaint.setPen(Qt::red);

	// auto size = page->pageSize();

	double scaleFactor = m_DPI / 72.0;

	for(int i=0; i < highlightRect.size(); i++)
		qImagePaint.drawRect(highlightRect[i].x() * scaleFactor, highlightRect[i].y() * scaleFactor, highlightRect[i].width() * scaleFactor, highlightRect[i].height() * scaleFactor);

	qImagePaint.end();
	m_img->setPixmap(QPixmap::fromImage(m_currentImage));
}

void Dodo::searchDocument(QString str)
{
	QList<QRectF> highlightRect;
	for(int i=0; i < m_pCount; i++)
	{
		auto page = m_doc->page(i);
		highlightRect = page->search(str);
		if(highlightRect.size() > 0)
		{
			gotoPage(i);
			break;
		}
	}
}

void Dodo::Escape()
{
	if(m_searchMode)
		m_searchMode = false;
	qDebug() << "DD";
}
