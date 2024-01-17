#pragma once

#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QStackedLayout>
#include <qt6/QtWidgets/QStackedWidget>
#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtWidgets/QWidget>
#include <qt6/QtWidgets/QScrollArea>
#include <qt6/QtWidgets/QScrollBar>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QHBoxLayout>
#include <qt6/QtWidgets/QLineEdit>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QTreeWidget>
#include <qt6/QtWidgets/QTreeWidgetItem>

#include <qt6/QtCore/QEvent>
#include <qt6/QtCore/QFile>
#include <qt6/QtGui/QPainter>

#include <qt6/QtGui/QKeyEvent>
#include <qt6/QtGui/QShortcut>
#include <qt6/QtGui/QKeySequence>
#include <qt6/QtGui/QGenericMatrix>
#include <qt6/QtGui/QTransform>


#include <poppler/qt6/poppler-qt6.h>

#include "MessageBar.hpp"

class CommandBar;
class StatusBar;
class TOC;
class TreeWidget;

enum Direction
{
    Left,
    Right,
    Up,
    Down
};

class Dodo : public QMainWindow
{

    Q_OBJECT

public:
    Dodo(int argc, char **argv);
    ~Dodo();

    void Init();
    void OpenFile(QString fileName);
	void OpenFileDialog();
    void CloseFile();
    inline void ZoomReset();
    inline void ZoomIn();
    inline void ZoomOut();
    void scaleImage(double factor);
    void handleKeys();
    void pageSettings();
    void renderPage(int page = Dodo::m_pageNumber);
    void cachePage(int page);
    inline bool isAlreadyCached(int page);
    void removeCachedPage(int page);
    void checkAndRenderPage(int page = Dodo::m_pageNumber);
    bool nextPage();
    bool prevPage();
    inline int getCurrentPage() const { return m_pageNumber; }
    void setCurrentPage(int page);
	void gotoPage(int page);
    bool isPageValid(int page);
    void Scroll(Direction direction);
    void FitToHeight();
    void FitToWidth();
    void FitToWindow();
    void InitStatusBar();
    void InitCommandBar();
    void InitTOC();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void begOfDocument();
    void endOfDocument();
    void toggleVHScrollBars();
    void toggleTOC();
    void handleVscroll(int value);
    void toggleDarkMode();
    void processOutline();
    void invertToColor();
    void toggleRecolor();
    void getLinks();
    void pageScrollTop();
    void pageScrollBottom();
	void searchPage(const QString str, const int page = m_pageNumber);
    void searchDocument(QString str);
	void Escape();

    friend class StatusBar;
    friend class CommandBar;
    friend class TreeWidget;

signals:
    void currentPageChanged(int page = Dodo::m_pageNumber);
    void documentChanged(Poppler::Document *doc);


private slots:
    void pageChanged(int page);
    void handleDocumentChanged(Poppler::Document *doc);

private:

    QVBoxLayout *m_layout = new QVBoxLayout();
    QWidget *m_widget = new QWidget();
    QStackedWidget *m_stackedWidget = new QStackedWidget();
    QWidget *m_mainWidget = new QWidget();
    QVBoxLayout *m_mainLayout = new QVBoxLayout();
    QScrollArea *m_scrollArea = new QScrollArea();
    QWidget *m_scrollWidget = new QWidget();
    QVBoxLayout *m_scrollWidgetLayout = new QVBoxLayout();
	QSize m_imageScale;

    QImage m_currentImage;
    std::unique_ptr<Poppler::Document> m_doc;
    std::unique_ptr<Poppler::Page> m_page;

    QLabel *m_img = new QLabel();
    QMap<int, QPixmap> m_img_map;
    QLabel *m_pageNumberLabel = new QLabel(),
        *m_docNameLabel = new QLabel(),
        *m_pageCountLabel = new QLabel(),
        *m_pageCountDividerLabel = new QLabel("/");

    double m_res = 4.0, m_zoomFactor = 1.0;
    int m_pCount = -1,
        m_DPI = 250,
        m_yScrollAmount = 50,
        m_xScrollAmount = 50,
        m_argc;

    char **m_argv;
    static int m_pageNumber;
    QScrollBar *m_HScrollBar = m_scrollArea->horizontalScrollBar(),
               *m_VScrollBar = m_scrollArea->verticalScrollBar();

    StatusBar *m_statusBar;
    CommandBar *m_commandBar;
    TOC *m_tocWidget;
    MessageBar *m_messageBar = new MessageBar();

    bool m_scrollBarsShown = true,
        m_darkMode = false,
        m_recolor = false,
		m_searchMode = false;

    QVector<Poppler::OutlineItem> m_outlines;
    unsigned int m_outlineSize;
    QString $HOME = getenv("HOME");

    QString m_chapterFont = "Rajdhani Semibold",
        m_unitFont = "Roboto Mono";

    QColor tmpColor;

	QList<QList<QRectF>> m_searchResultList;
};

extern Dodo* dodo;
