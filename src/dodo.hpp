#include <qt6/QtCore/QDebug>
#include <qt6/QtGui/QShortcut>
#include <qt6/QtGui/QKeySequence>
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QFileDialog>
#include <qt6/QtWidgets/QMainWindow>
#include <qt6/QtWidgets/QRubberBand>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtGui/QRgb>
#include <qt6/QtGui/QImage>
#include <qt6/QtGui/QPainter>
#include <qt6/QtGui/QPen>
#include <qt6/QtGui/QColor>
#include <qt6/QtWidgets/QScrollArea>
#include <qt6/QtWidgets/QScrollBar>
#include <qt6/QtWidgets/QInputDialog>
#include <qt6/QtWidgets/QMenu>
#include <qt6/QtWidgets/QMenuBar>
#include <qt6/QtGui/QMouseEvent>
#include <mupdf/fitz.h>
#include "statusbar.hpp"
#include "commandbar.hpp"
#include "label.hpp"

enum AnnotType
{
    UNDERLINE,
    HIGHLIGHT,
    STRIKETHROUGH,
};

class Dodo : public QMainWindow {

public:
    Dodo(int argc, char **argv, QWidget *parent = nullptr);
    ~Dodo();

    void Annotate();
    void FitToWidth();
    void FitToHeight();
    void FitToWindow();
    void GotoPage(int pinterval);
    bool Open(QString filename, int page_number = 0);
    void Render(float dpi = 72.0);
    void Rotate(float angle);
    void ResetView();
    void Reset();
    void ClearSearch();
    void SetKeyBinds();
    void ScrollVertical(int direction, int amount);
    void ScrollHorizontal(int direction, int amount);
    void SearchMode();
    void ShowPage();
    int SearchText(QString text);
    void Zoom(float rate);
    void ZoomReset();
    void HandleMenubar();
    void HandleActions();
    void HandleFileMenuActions();
    void HandleEditMenuActions();
    void HandleViewMenuActions();
    void HandlePassword();
    int TotalSearchCount(QString text);
    void SearchReset();
    void Search_Goto_Next();
    void Search_Goto_Prev();
    void NextPage();
    void PrevPage();
    void GotoFirstPage();
    void GotoLastPage();
    int GetCurrentPage();
    void Search_Highlight_Next_Item();
    void Search_Highlight_Prev_Item();

    void SetAnnotColor(const QColor color);
    QColor GetAnnotColor();
    void SetAnnotType(AnnotType type);
    AnnotType GetAnnotType();

    bool isPDF();

    void HighlightRect(const QRect &);

private:

    fz_quad convertRectToQuad(const QRect &rect);

    fz_context *m_ctx;
    fz_document *m_doc;
    fz_pixmap *m_pix;
    fz_page *m_page;
    fz_device *m_dev;

    QPoint rubberband_offset;

    int m_cur_page_num = 1,
        m_page_count = -1,
        m_scroll_len = 10,
        m_search_count = -1,
        m_search_index = -1;
    int HIT_MAX_COUNT = 1000;

    float m_zoom = 1.0f, m_rotate = 0.0f;
    QPoint m_annot_start_pos, m_annot_end_pos;
    QString m_filename,
            m_search_text;
    fz_matrix m_ctm = fz_identity;
    int m_x, m_y;
    QImage m_image, m_g_image;
    QScrollArea *m_scrollarea = new QScrollArea();
    QScrollBar* m_vscroll = m_scrollarea->verticalScrollBar();
    QScrollBar* m_hscroll = m_scrollarea->horizontalScrollBar();
    Label *m_label = new Label();
    QVBoxLayout *m_layout = new QVBoxLayout();
    QWidget *m_widget = new QWidget();
    StatusBar *m_statusbar = new StatusBar();
    CommandBar *m_commandbar = new CommandBar();
    QColor m_annot_color;
    AnnotType m_annot_type = AnnotType::HIGHLIGHT;
    bool m_is_annotate_active = true, m_fit_to_width_mode = true;
    QMenuBar *m_menubar;
    QMenu *m_menu__file, *m_menu__edit, *m_menu__view, *m_menu__tools, *m_menu__about;

    QAction *m_action__file_open, *m_action__file_open_recents,
            *m_action__file_properties, *m_action__file_close,
            *m_action__file_quit;
};
