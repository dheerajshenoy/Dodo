#include "dodo.hpp"
#include "BrowseLinkItem.hpp"
#include "GraphicsView.hpp"
#include "PropertiesWidget.hpp"
#include <algorithm>


dodo::dodo() noexcept
{
    setAttribute(Qt::WA_NativeWindow); // This is necessary for DPI updates
}

dodo::~dodo() noexcept
{}

void dodo::construct() noexcept
{
    initGui();
    initConfig();
    initMenubar();
    initDB();
    DPI_FRAC = m_dpi / m_low_dpi;

    if (m_load_default_keybinding)
    {
        qDebug() << "Loading default keybindings";
        initKeybinds();
    }

    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());

#ifndef NDEBUG
    // openFile("~/Downloads/basic-link-1.pdf");
    // openFile("~/Scott Dodelson, Fabian Schmidt - Modern Cosmology-Academic Press (2020).pdf"); // FOR DEBUG PURPOSE ONLY
#endif

    m_page_history_list.reserve(m_page_history_limit);
    initConnections();
    m_pix_item->setScale(m_scale_factor);
    this->show();
}

void dodo::initMenubar() noexcept
{

    // --- File Menu ---
    QMenu *fileMenu = m_menuBar->addMenu("File");
    fileMenu->addAction(QString("Open File\t%1").arg(m_shortcuts_map["open_file"]),
                        this, &dodo::OpenFile);
    m_actionFileProperties = fileMenu->addAction(QString("File Properties\t%1").arg(m_shortcuts_map["file_properties"]),
                                                this, &dodo::FileProperties);
    m_actionCloseFile = fileMenu->addAction(QString("Close File\t%1").arg(m_shortcuts_map["close_file"]),
                                            this, &dodo::CloseFile);
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", this, &QMainWindow::close);

    // --- View Menu ---
    QMenu *viewMenu = m_menuBar->addMenu("View");
    m_actionZoomIn = viewMenu->addAction(QString("Zoom In\t%1").arg(m_shortcuts_map["zoom_in"]),
                                         this, &dodo::ZoomIn);
    m_actionZoomOut = viewMenu->addAction(QString("Zoom Out\t%1").arg(m_shortcuts_map["zoom_out"]),
                                          this, &dodo::ZoomOut);
    viewMenu->addSeparator();

    // Zoom Mode Actions (exclusive)
    QActionGroup *zoomModeGroup = new QActionGroup(this);
    zoomModeGroup->setExclusive(true);

    m_fitMenu = new QMenu();
    m_fitMenu = viewMenu->addMenu("Fit");

    m_actionFitNone = m_fitMenu->addAction(QString("None\t%1").arg(m_shortcuts_map["fit_none"]),
                                            this, &dodo::FitNone);
    m_actionFitNone->setCheckable(true);
    zoomModeGroup->addAction(m_actionFitNone);

    m_actionFitWidth = m_fitMenu->addAction(QString("Width\t%1").arg(m_shortcuts_map["fit_width"]),
                                           this, &dodo::FitWidth);
    m_actionFitWidth->setCheckable(true);
    zoomModeGroup->addAction(m_actionFitWidth);

    m_actionFitHeight = m_fitMenu->addAction(QString("Height\t%1").arg(m_shortcuts_map["fit_height"]),
                                            this, &dodo::FitHeight);
    m_actionFitHeight->setCheckable(true);
    zoomModeGroup->addAction(m_actionFitHeight);

    m_actionFitWindow = m_fitMenu->addAction(QString("Window\t%1").arg(m_shortcuts_map["fit_window"]),
                                            this, &dodo::FitWindow);
    m_actionFitWindow->setCheckable(true);
    zoomModeGroup->addAction(m_actionFitWindow);

    m_fitMenu->addSeparator();

    // Auto Resize toggle (independent)
    m_actionAutoresize = m_fitMenu->addAction(QString("Auto Resize\t%1").arg(m_shortcuts_map["auto_resize"]),
                                             this, &dodo::ToggleAutoResize);
    m_actionAutoresize->setCheckable(true);
    m_actionAutoresize->setChecked(true);  // default on or off

    viewMenu->addSeparator();

    m_actionToggleOutline = viewMenu->addAction(QString("Outline\t%1").arg(m_shortcuts_map["outline"]),
                                             this, &dodo::TableOfContents);

    m_actionToggleMenubar = viewMenu->addAction(QString("Menubar\t%1").arg(m_shortcuts_map["toggle_menubar"]),
                                             this, &dodo::ToggleMenubar);
    m_actionToggleMenubar->setCheckable(true);
    m_actionToggleMenubar->setChecked(!m_menuBar->isHidden());

    m_actionTogglePanel = viewMenu->addAction(QString("Panel\t%1").arg(m_shortcuts_map["toggle_panel"]),
                                             this, &dodo::TogglePanel);
    m_actionTogglePanel->setCheckable(true);
    m_actionTogglePanel->setChecked(!m_panel->isHidden());

    // --- Navigation Menu ---
    QMenu *navMenu = m_menuBar->addMenu("Navigation");
    m_actionFirstPage = navMenu->addAction(QString("First Page\t%1").arg(m_shortcuts_map["first_page"]),
                                           this, &dodo::FirstPage);

    m_actionPrevPage = navMenu->addAction(QString("Previous Page\t%1").arg(m_shortcuts_map["prev_page"]),
                                          this, &dodo::PrevPage);

    m_actionNextPage = navMenu->addAction(QString("Next Page\t%1").arg(m_shortcuts_map["next_page"]),
                                          this, &dodo::NextPage);
    m_actionLastPage = navMenu->addAction(QString("Last Page\t%1").arg(m_shortcuts_map["last_page"]),
                                          this, &dodo::LastPage);

    m_actionPrevLocation = navMenu->addAction(QString("Previous Location\t%1").arg(m_shortcuts_map["prev_location"]),
                                          this, &dodo::GoBackHistory);

    QMenu *helpMenu = m_menuBar->addMenu("Help");
    m_actionAbout = navMenu->addAction(QString("About\t%1").arg(m_shortcuts_map["about"]),
                                       this, &dodo::ShowAbout);

    updateUiEnabledState();
}

void dodo::initConnections() noexcept
{
    connect(m_model, &Model::searchResultsReady, this, [&](const QMap<int, QList<Model::SearchResult>> &maps,
                                                           int matchCount) {
            if (matchCount == 0)
            {
            QMessageBox::information(this, "Search Result",
                                     QString("No match found for {}").arg(m_last_search_term));
            return;
            }
            m_searchRectMap = maps;
            m_panel->setSearchCount(matchCount);
            auto page = maps.firstKey();
            jumpToHit(page, 0);
            });

    connect(m_gview, &GraphicsView::highlightDrawn, this, [=](const QRectF &pdfRect) {
        m_model->addHighlightAnnotation(m_pageno, pdfRect);
    });

    connect(m_model, &Model::horizontalFitRequested, this, [&](int pageno, const BrowseLinkItem::Location &location) {
        qDebug() << "HFIT";
        gotoPage(pageno);
        FitWidth();
        scrollToNormalizedTop(location.y);
    });

    connect(m_model, &Model::verticalFitRequested, this, [&](int pageno, const BrowseLinkItem::Location &location) {
        qDebug() << "VFIT";
        gotoPage(pageno);
        FitHeight();
        scrollToNormalizedTop(location.y);
    });

    connect(m_model, &Model::jumpToPageRequested, this, [&](int pageno) {
        gotoPage(pageno);
        TopOfThePage();
    });

    connect(m_model, &Model::jumpToLocationRequested, this, [&](int pageno, const BrowseLinkItem::Location &loc) {
        gotoPage(pageno);
        scrollToXY(loc.x, loc.y);
    });

}

void dodo::scrollToXY(float x, float y) noexcept
{
    if (!m_pix_item || !m_gview)
        return;

    m_gview->centerOn(QPointF(x * DPI_FRAC, y * DPI_FRAC));
}

void dodo::initDB() noexcept
{
    if (!m_remember_last_visited)
        return;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_config_dir.filePath("last_pages.db"));
    db.open();

    QSqlQuery query;

    // FIXME: Maybe add some unique hashing so that this works even when you move a file
    query.exec("CREATE TABLE IF NOT EXISTS last_visited ("
               "file_path TEXT PRIMARY KEY, "
               "page_number INTEGER, "
               "last_accessed TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");
}

void dodo::initConfig() noexcept
{
    m_config_dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    auto config_file_path = m_config_dir.filePath("config.toml");

    toml::table toml;

    try
    {
        toml = toml::parse_file(config_file_path.toStdString());
    } catch (std::exception &e)
    {
        QMessageBox::critical(this, "Error in configuration file",
                              QString("There are one or more error(s) in your config file:\n%1\n\nLoading default config.")
                              .arg(e.what()));
        return;
    }

    auto ui = toml["ui"];
    m_panel->setHidden(!ui["panel"].value_or(true));

    m_menuBar->setHidden(!ui["menubar"].value_or(true));

    if (ui["fullscreen"].value_or(false))
        ToggleFullscreen();

    auto vscrollbar_shown = ui["vscrollbar"].value_or(true);
    auto vscrollbar = m_gview->verticalScrollBar();
    vscrollbar->setVisible(vscrollbar_shown);

    auto hscrollbar_shown = ui["hscrollbar"].value_or(true);
    auto hscrollbar = m_gview->horizontalScrollBar();
    hscrollbar->setVisible(hscrollbar_shown);

    auto auto_hide_hscrollbar = ui["auto_hide_hscrollbar"].value_or(true);
    auto auto_hide_vscrollbar = ui["auto_hide_vscrollbar"].value_or(true);

    if (auto_hide_hscrollbar)
        m_gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    if (auto_hide_vscrollbar)
        m_gview->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_full_file_path_in_panel = ui["full_file_path_in_panel"].value_or(false);
    m_scale_factor = ui["zoom_level"].value_or(1.0);
    bool compact = ui["compact"].value_or(false);
    m_model->setLinkBoundaryBox(ui["link_boundary_box"].value_or(false));

    m_window_title = ui["window_title"].value_or("{} - dodo");
    m_window_title.replace("{}", "%1");

    auto colors = toml["colors"];

    m_colors["search_index"] = QColor::fromString(colors["search_index"].value_or("#3daee944")).rgba();
    m_colors["search_match"] = QColor::fromString(colors["search_match"].value_or("#FFFF8844")).rgba();
    m_colors["accent"] = QColor::fromString(colors["accent"].value_or("#FF500044")).rgba();
    m_colors["background"] = QColor::fromString(colors["background"].value_or("#FFFFFF")).rgba();
    m_colors["link_hint_fg"] = QColor::fromString(colors["link_hint_fg"].value_or("#000000")).rgba();
    m_colors["link_hint_bg"] = QColor::fromString(colors["link_hint_bg"].value_or("#FFFF00")).rgba();

    m_model->setLinkHintBackground(m_colors["link_hint_bg"]);
    m_model->setLinkHintForeground(m_colors["link_hint_fg"]);
    m_gview->setBackgroundBrush(QColor::fromRgba(m_colors["background"]));

    auto rendering = toml["rendering"];
    m_dpi = rendering["dpi"].value_or(300.0);
    m_model->setDPI(m_dpi);
    m_model->setLowDPI(m_low_dpi);
    int cache_pages = rendering["cache_pages"].value_or(50);

    auto behavior = toml["behavior"];
    m_remember_last_visited = behavior["remember_last_visited"].value_or(true);
    // m_prefetch_enabled = behavior["enable_prefetch"].value_or(true);
    // m_prefetch_distance = behavior["prefetch_distance"].value_or(2);
    m_page_history_limit = behavior["page_history"].value_or(100);
    m_model->setAntialiasingBits(behavior["antialasing_bits"].value_or(8));
    m_auto_resize = behavior["auto_resize"].value_or(false);
    m_zoom_by = behavior["zoom_factor"].value_or(1.25);
    if (behavior["invert_mode"].value_or(false))
        m_model->invertColor();

    if (behavior["icc_color_profile"].value_or(true))
        m_model->enableICC();

    auto initial_fit = behavior["initial_fit"].value_or("width");

    if (strcmp(initial_fit, "height") == 0)
        m_initial_fit = FitMode::Height;
    else if (strcmp(initial_fit, "width") == 0)
        m_initial_fit = FitMode::Width;
    else if (strcmp(initial_fit, "window") == 0)
        m_initial_fit = FitMode::Window;
    else
        m_initial_fit = FitMode::None;

    if (toml.contains("keybindings"))
    {
        qDebug() << "Loading custom keybindings";
        m_load_default_keybinding = false;
        auto keys = toml["keybindings"];
        m_actionMap = {
            { "about", [this]() { ShowAbout(); } },
            { "link_hint_visit", [this]() { VisitLinkKB(); } },
            { "link_hint_copy", [this]() { CopyLinkKB(); } },
            { "outline", [this]() { TableOfContents(); } },
            { "rotate_clock", [this]() { RotateClock(); } },
            { "rotate_anticlock", [this]() { RotateAntiClock(); } },
            { "prev_location", [this]() { GoBackHistory(); } },
            { "scroll_down", [this]() { ScrollDown(); } },
            { "scroll_up", [this]() { ScrollUp(); } },
            { "scroll_left", [this]() { ScrollLeft(); } },
            { "scroll_right", [this]() { ScrollRight(); } },
            { "invert_color", [this]() { InvertColor(); } },
            { "search", [this]() { Search(); } },
            { "search_next", [this]() { nextHit(); } },
            { "search_prev", [this]() { prevHit(); } },
            { "search_page", [this]() { SearchPage(); } },
            { "next_page", [this]() { NextPage(); } },
            { "prev_page", [this]() { PrevPage(); } },
            { "first_page", [this]() { FirstPage(); } },
            { "last_page", [this]() { LastPage(); } },
            { "zoom_in", [this]() { ZoomIn(); } },
            { "zoom_out", [this]() { ZoomOut(); } },
            { "zoom_reset", [this]() { ZoomReset(); } },
            { "annot_highlight", [this]() { ToggleHighlight(); } },
            { "fullscreen", [this]() { ToggleFullscreen(); } },
            { "file_properties", [this]() { FileProperties(); } },
            { "open_file", [this]() { OpenFile(); } },
            { "close_file", [this]() { CloseFile(); } },
            { "fit_width", [this]() { FitWidth(); } },
            { "fit_height", [this]() { FitHeight(); } },
            { "fit_window", [this]() { FitWindow(); } },
            { "auto_resize", [this]() { ToggleAutoResize(); } },
            { "toggle_menubar", [this]() { ToggleMenubar(); } },
            { "toggle_panel", [this]() { TogglePanel(); } },

        };

        for (auto &[action, value] : *keys.as_table())
        {
            if (value.is_value())
                setupKeybinding(QString::fromStdString(std::string(action.str())),
                                QString::fromStdString(value.value_or<std::string>("")));
        }
    }

    if (compact)
    {
        m_layout->setContentsMargins(0, 0, 0, 0);
        // m_panel->layout()->setContentsMargins(0, 0, 0, 0);
        m_panel->setContentsMargins(0, 0, 0, 0);
        this->setContentsMargins(0, 0, 0, 0);
    }

    m_pixmapCache.setMaxCost(cache_pages);
    m_highResCache.setMaxCost(cache_pages);
}

void dodo::initKeybinds() noexcept
{
    std::vector<std::pair<QString, std::function<void()>>> shortcuts = {

        { "b", [this]() { GotoPage(); } },
        { "f", [this]() { VisitLinkKB(); } },
        { "Ctrl+s", [this]() { SaveFile(); } },
        { "Alt+1", [this]() { ToggleHighlight(); } },
        { "t", [this]() { TableOfContents(); } },
        { "/", [this]() { Search(); } },
        { "n", [this]() { nextHit(); } },
        { "Shift+n", [this]() { prevHit(); } },
        { "Ctrl+o", [this]() { GoBackHistory(); } },
        { "o", [this]() { OpenFile(); } },
        { "j", [this]() { ScrollDown(); } },
        { "k", [this]() { ScrollUp(); } },
        { "h", [this]() { ScrollLeft(); } },
        { "l", [this]() { ScrollRight(); } },
        { "Shift+j", [this]() { NextPage(); } },
        { "Shift+k", [this]() { PrevPage(); } },
        { "g,g", [this]() { FirstPage(); } },
        { "Shift+g", [this]() { LastPage(); } },
        { "0", [this]() { ZoomReset(); } },
        { "=", [this]() { ZoomIn(); } },
        { "-", [this]() { ZoomOut(); } },
        { "<", [this]() { RotateAntiClock(); } },
        { ">", [this]() { RotateClock(); } },
    };

    for (const auto& [key, func] : shortcuts) {
        auto *sc = new QShortcut(QKeySequence(key), this);
        connect(sc, &QShortcut::activated, func);
    }

}

void dodo::initGui() noexcept
{
    QWidget *widget = new QWidget();
    // Panel
    m_panel = new Panel(this);
    widget->setLayout(m_layout);
    m_layout->addWidget(m_gview);
    m_layout->addWidget(m_panel);
    m_gview->setRenderHint(QPainter::Antialiasing);
    m_gview->setScene(m_gscene);
    this->setCentralWidget(widget);

    // m_gview->setPixmapItem(m_pix_item);
    m_gscene->addItem(m_pix_item);

    m_model = new Model(m_gscene);
    // connect(m_model, &Model::imageRenderRequested, this, &dodo::handleRenderResult);
    // Menu Bar

    auto win = m_gview->window()->windowHandle();
    if (win)
    {
        connect(win, &QWindow::screenChanged, m_gview, [&](QScreen *screen) {
            auto dpr = m_gview->window()->devicePixelRatioF();
            m_model->setDPR(dpr);
            m_dpr = dpr;
            m_inv_dpr = 1 / dpr;
            renderPage(m_pageno, true);
        });
    }

    m_menuBar = this->menuBar(); // initialize here so that the config visibility works
}

void dodo::handleRenderResult(int pageno, QImage image)
{
    if (pageno != m_pageno || image.isNull())
    {
        qDebug() << "Old, ignoring";
        return;
    }

    QPixmap pix = QPixmap::fromImage(image);

    m_pixmapCache.insert(pageno, new QPixmap(pix));
    m_pix_item->setScale(m_scale_factor);
    m_pix_item->setPixmap(pix);

    // Normalize scale: use logical size instead of raw pixels

    // if (m_prefetch_enabled)
    // {
    //     prefetchAround(m_pageno);
    // }
    m_panel->setPageNo(m_pageno + 1);
    renderLinks();
}

void dodo::updateUiEnabledState() noexcept
{
    const bool hasOpenedFile = m_model->valid();

    m_actionZoomIn->setEnabled(hasOpenedFile);
    m_actionZoomOut->setEnabled(hasOpenedFile);
    m_actionFirstPage->setEnabled(hasOpenedFile);
    m_actionPrevPage->setEnabled(hasOpenedFile);
    m_actionNextPage->setEnabled(hasOpenedFile);
    m_actionLastPage->setEnabled(hasOpenedFile);
    m_actionFileProperties->setEnabled(hasOpenedFile);
    m_actionCloseFile->setEnabled(hasOpenedFile);

}


void dodo::OpenFile() noexcept
{
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "Open File",
                                                    "",
                                                    "PDF Files (*.pdf);; All Files (*)");
    if (filepath.isEmpty())
        return;

    openFile(filepath);
}

void dodo::CloseFile() noexcept
{
    if (!m_model->valid())
        return;

    if (m_model->hasUnsavedChanges())
    {
        auto close = QMessageBox::question(this, "Unsaved changes detected",
                                            "There are unsaved changes in this document. Do you really want to close this file ?");
        if (close == QMessageBox::StandardButton::No)
            return;
    }
    m_model->closeFile();
    m_filename.clear();
    if (m_panel->searchMode())
        m_panel->setSearchMode(false);
    if (m_highlights_present)
    {
        clearHighlights();
        clearIndexHighlights();
    }
    m_panel->setFileName("");

    if (m_owidget)
    {
        m_owidget->close();
        m_owidget->deleteLater();
        m_owidget = nullptr;
    }

    if (m_propsWidget)
    {
        m_owidget->close();
        m_propsWidget->deleteLater();
        m_propsWidget = nullptr;
    }
}

/* Function for opening the file using the model.
 For internal usage only */
void dodo::openFile(const QString &fileName) noexcept
{
    m_filename = fileName;
    m_filename.replace("~", QString::fromStdString(getenv("HOME")));
    QList<QGraphicsItem*> items = m_pix_item->childItems();
    for (const auto &item : items)
    m_gscene->removeItem(item);

    qDeleteAll(items);

    if (!QFile::exists(m_filename))
    {
        qWarning("file does not exist!: ");
        return;
    }

    if (!m_model->openFile(m_filename))
        return;

    if (m_model->passwordRequired())
        if(!askForPassword())
            return;

    m_pageno = -1;
    m_total_pages = m_model->numPages();
    m_panel->setTotalPageCount(m_total_pages);

    if (m_panel->searchMode())
        m_panel->setSearchMode(false);

    // Set the window title
    this->setWindowTitle(QString(m_window_title).arg(fz_basename(CSTR(m_filename))));

    if (m_full_file_path_in_panel)
        m_panel->setFileName(m_filename);
    else
        m_panel->setFileName(fz_basename(CSTR(m_filename)));

    if (m_remember_last_visited)
    {
        QSqlQuery q;
        q.prepare("SELECT page_number FROM last_visited WHERE file_path = ?");
        q.addBindValue(m_filename);
        q.exec();

        if (q.next()) {
            int page = q.value(0).toInt();
            gotoPage(page);
        } else {
            gotoPage(0);
        }
    } else gotoPage(0);

    updateUiEnabledState();

    if (m_initial_fit != FitMode::None)
    {
        QTimer::singleShot(10, this, [this]() {
            switch(m_initial_fit)
            {
                case FitMode::Height: FitHeight(); break;
                case FitMode::Width: FitWidth(); break;
                case FitMode::Window: FitWindow(); break;
                case FitMode::None: break;
            }
        });
    }

}

bool dodo::askForPassword() noexcept
{
    bool ok;
    auto pwd = QInputDialog::getText(this, "Document is locked", "Enter password",
                                     QLineEdit::EchoMode::Password, QString(), &ok);
    if (!ok)
        return false;

    auto correct = m_model->authenticate(pwd);
    if (correct)
        return true;
    else
    {
        QMessageBox::critical(this, "Password", "Password is incorrect");
        return false;
    }
}

void dodo::RotateClock() noexcept
{
    if (!m_pix_item)
        return;

    m_rotation = (m_rotation + 90) % 360;
    renderPage(m_pageno, true);
}

void dodo::RotateAntiClock() noexcept
{
    if (!m_pix_item)
        return;

    m_rotation = (m_rotation + 270) % 360;
    renderPage(m_pageno, true);
}

void dodo::gotoPage(const int &pageno) noexcept
{
    if (!m_model->valid())
    {
        qWarning("Trying to go to page but no document loaded");
        return;
    }

    if (pageno < 0 || pageno >= m_total_pages) {
        qWarning("Page number %d out of range", pageno);
        return;
    }

    if (pageno == m_pageno) {
        return; // No-op
    }

    // boundary condition
    if (!m_suppressHistory && m_pageno >= 0)
    {
        m_page_history_list.push_back(m_pageno);
        if (m_page_history_list.size() > m_page_history_limit)
            m_page_history_list.removeFirst();
    }

    gotoPageInternal(pageno);
}

void dodo::gotoPageInternal(const int &pageno) noexcept
{

    if (pageno < 0 || pageno >= m_total_pages)
    {
        qWarning("Page number %d out of range", pageno);
        return;
    }

    m_pageno = pageno;

    // TODO: Handle file content change detection

    // if (m_highResCache.contains(pageno))
    // {
    //     QPixmap *cached = m_highResCache.object(pageno);
    //     m_pix_item->setPixmap(*cached);
    //     m_pix_item->setScale(m_scale_factor);
    //     return;
    // }

    if (m_highlights_present)
    {
        clearHighlights();
        clearIndexHighlights();
    }

    renderPage(pageno, false);
}

void dodo::renderPage(int pageno, bool renderonly) noexcept
{

    // if (m_highResCache.contains(pageno))
    // {
    //     m_pix_item->setPixmap(*m_highResCache.object(pageno));
    //     m_pix_item->setScale(DPI_FRAC * m_scale_factor);
    //     return;
    // }

    auto pix = m_model->renderPage(pageno, m_scale_factor, m_rotation, renderonly);
    renderPixmap(pix);
}

void dodo::renderPixmap(const QPixmap &pix) noexcept
{
    m_pix_item->setPixmap(pix);
    m_panel->setPageNo(m_pageno + 1);
}

bool dodo::isPrefetchPage(int page, int currentPage) noexcept
{
    // Prefetch next and previous page only
    return page == currentPage + 1 || page == currentPage - 1;
}

void dodo::cachePage(int pageno) noexcept
{
    if (m_highResCache.contains(pageno) || m_pixmapCache.contains(pageno))
    {
        return;
    }

    m_model->renderPage(pageno, m_scale_factor, m_rotation);
    // auto *pix = new QPixmap(QPixmap::fromImage(img));
    // m_highResCache.insert(pageno, pix);
}

void dodo::prefetchAround(int currentPage) noexcept
{
    for (int offset = -m_prefetch_distance; offset <= m_prefetch_distance; ++offset) {
        int page = currentPage + offset;
        if (page >= 0 && page < m_total_pages) {
            cachePage(page);
        }
    }
}

void dodo::FirstPage() noexcept
{
    gotoPage(0);
    TopOfThePage();
}

void dodo::LastPage() noexcept
{
    gotoPage(m_total_pages - 1);
}


void dodo::NextPage() noexcept
{
    gotoPage(m_pageno + 1);
}

void dodo::PrevPage() noexcept
{
    gotoPage(m_pageno - 1);
}


void dodo::ZoomIn() noexcept
{
    // if (m_scale_factor < 5.0)
    // {
    m_scale_factor *= m_zoom_by;
    // m_gview->scale(1.1, 1.1);
    zoomHelper();
    // }
}

void dodo::ZoomReset() noexcept
{
    m_scale_factor = 1.0;
    // m_gview->resetTransform();
    zoomHelper();
}


void dodo::ZoomOut() noexcept
{
    if (m_scale_factor * 1 / m_zoom_by != 0)
    {
        // m_gview->scale(0.9, 0.9);
        m_scale_factor *= 1 / m_zoom_by;
        // m_gview->scale(1.1, 1.1);
        zoomHelper();
    }
}

void dodo::Zoom(float factor) noexcept
{
    // TODO: Add constraints here

    // m_gview->scale(factor, factor);
    m_scale_factor = factor;
    zoomHelper();
}

void dodo::zoomHelper() noexcept
{
    renderPage(m_pageno, true);
    if (m_highlights_present)
    {
        highlightSingleHit();
        highlightHitsInPage();
    }
    m_gview->setSceneRect(m_pix_item->boundingRect());
    auto vscrollbar = m_gview->verticalScrollBar();
    vscrollbar->setValue(vscrollbar->value());
}

void dodo::ScrollDown() noexcept
{
    auto vscrollbar = m_gview->verticalScrollBar();
    vscrollbar->setValue(vscrollbar->value() + 50);
}

void dodo::ScrollUp() noexcept
{
    auto vscrollbar = m_gview->verticalScrollBar();
    vscrollbar->setValue(vscrollbar->value() - 50);
}

void dodo::ScrollLeft() noexcept
{
    auto hscrollbar = m_gview->horizontalScrollBar();
    hscrollbar->setValue(hscrollbar->value() - 50);
}

void dodo::ScrollRight() noexcept
{
    auto hscrollbar = m_gview->horizontalScrollBar();
    hscrollbar->setValue(hscrollbar->value() + 50);
}

void dodo::FitHeight() noexcept
{
    if (!m_pix_item || m_pix_item->pixmap().isNull())
        return;

    int pixmapHeight = m_pix_item->pixmap().height();
    int viewHeight = m_gview->viewport()->height() * m_gview->window()->devicePixelRatioF();

    qreal scale = static_cast<qreal>(viewHeight) / pixmapHeight;
    m_scale_factor *= scale;
    zoomHelper();
    setFitMode(FitMode::Height);
}

void dodo::FitWidth() noexcept
{
    if (!m_pix_item || m_pix_item->pixmap().isNull())
        return;

    int pixmapWidth = m_pix_item->pixmap().width();
    int viewWidth = m_gview->viewport()->width() * m_gview->window()->devicePixelRatioF();

    qreal scale = static_cast<qreal>(viewWidth) / pixmapWidth;
    m_scale_factor *= scale;
    zoomHelper();
    setFitMode(FitMode::Width);
}

void dodo::FitWindow() noexcept
{
    const int pixmapWidth = m_pix_item->pixmap().width();
    const int pixmapHeight = m_pix_item->pixmap().height();
    const int viewWidth = m_gview->viewport()->width() * m_gview->window()->devicePixelRatioF();
    const int viewHeight = m_gview->viewport()->height() * m_gview->window()->devicePixelRatioF();

    // Calculate scale factors for both dimensions
    const qreal scaleX = static_cast<qreal>(viewWidth) / pixmapWidth;
    const qreal scaleY = static_cast<qreal>(viewHeight) / pixmapHeight;

    // Use the smaller scale to ensure the entire image fits in the window
    const qreal scale = std::min(scaleX, scaleY);

    m_scale_factor *= scale;
    zoomHelper();
    setFitMode(FitMode::Window);
}

void dodo::FitNone() noexcept
{
    setFitMode(FitMode::None);
}

void dodo::setFitMode(const FitMode &mode) noexcept
{
    m_fit_mode = mode;
    switch(m_fit_mode)
    {
        case FitMode::Height:
            m_panel->setFitMode("Fit Height");
            break;

        case FitMode::Width:
            m_panel->setFitMode("Fit Width");
            break;

        case FitMode::Window:
            m_panel->setFitMode("Fit Window");
            break;

        case FitMode::None:
            m_panel->setFitMode("");
            break;
    }
}

void dodo::renderLinks() noexcept
{
    m_model->renderLinks(m_pageno);
}

// Single page search
void dodo::search(const QString &term) noexcept
{
    m_last_search_term = term;
    // m_pdf_backend->search();
}

void dodo::jumpToHit(int page, int index)
{
    if (!m_searchRectMap.contains(page) ||
        index < 0 ||
        index >= m_searchRectMap[page].size())
        return;

    m_search_index = index;
    m_search_hit_page = page;

    gotoPage(page);  // Render page
    m_panel->setSearchIndex(m_searchRectMap[page][index].index + 1);

    highlightSingleHit();
    highlightHitsInPage();
}

void dodo::highlightHitsInPage()
{
    clearHighlights();

    auto results = m_searchRectMap[m_pageno];

    for (const auto &result : results)
    {
        fz_quad quad = result.quad;
        QRectF scaledRect = fzQuadToQRect(quad);
        auto *highlight = new QGraphicsRectItem(scaledRect);

        highlight->setBrush(QColor::fromRgba(m_colors["search_match"]));
        highlight->setPen(Qt::NoPen);
        highlight->setData(0, "searchHighlight");
        m_gscene->addItem(highlight);
        // m_gview->centerOn(highlight);
    }
    m_highlights_present = true;
}

void dodo::highlightSingleHit() noexcept
{
    if (m_highlights_present)
        clearIndexHighlights();

    fz_quad quad = m_searchRectMap[m_pageno][m_search_index].quad;

    QRectF scaledRect = fzQuadToQRect(quad);
    auto *highlight = new QGraphicsRectItem(scaledRect);
    highlight->setBrush(QColor::fromRgba(m_colors["search_index"]));
    highlight->setPen(Qt::NoPen);
    highlight->setData(0, "searchIndexHighlight");

    m_gscene->addItem(highlight);
    // m_gview->centerOn(highlight);
    m_highlights_present = true;
}

void dodo::clearIndexHighlights()
{
    for (auto item : m_gscene->items()) {
        if (auto rect = qgraphicsitem_cast<QGraphicsRectItem *>(item)) {
            if (rect->data(0).toString() == "searchIndexHighlight") {
                m_gscene->removeItem(rect);
                delete rect;
            }
        }
    }
    m_highlights_present = false;
}

void dodo::clearHighlights()
{
    for (auto item : m_gscene->items()) {
        if (auto rect = qgraphicsitem_cast<QGraphicsRectItem *>(item)) {
            if (rect->data(0).toString() == "searchHighlight") {
                m_gscene->removeItem(rect);
                delete rect;
            }
        }
    }
    m_highlights_present = false;
}

void dodo::Search() noexcept
{
    auto term = QInputDialog::getText(this, "Search", "Search for");
    m_searchRectMap.clear();
    m_search_index = -1;
    if (term.isEmpty() || term.isNull())
    {
        m_panel->setSearchMode(false);
        if (m_highlights_present)
        {
            clearHighlights();
            clearIndexHighlights();
        }
        return;
    }

    m_last_search_term = term;
    m_panel->setSearchMode(true);
    m_search_index = 0;

    m_model->searchAll(term, hasUpperCase(term));
}

void dodo::SearchPage() noexcept
{
    // TODO: Search only the current page
}

void dodo::nextHit()
{
    if (m_search_hit_page == -1)
        return;

    QList<int> pages = m_searchRectMap.keys();
    int currentPageIdx = pages.indexOf(m_search_hit_page);

    // Try next hit on current page
    if (m_search_index + 1 < m_searchRectMap[m_search_hit_page].size()) {
        jumpToHit(m_search_hit_page, m_search_index + 1);
        return;
    }

    // Try next page
    if (currentPageIdx + 1 < pages.size()) {
        if (!m_searchRectMap[pages[currentPageIdx + 1]].isEmpty())
            jumpToHit(pages[currentPageIdx + 1], 0);
    }
}

void dodo::prevHit()
{
    if (m_search_hit_page == -1) return;

    QList<int> pages = m_searchRectMap.keys();
    int currentPageIdx = pages.indexOf(m_search_hit_page);

    // Try previous hit on current page
    if (m_search_index - 1 >= 0) {
        jumpToHit(m_search_hit_page, m_search_index - 1);
        return;
    }

    // Try previous page
    if (currentPageIdx - 1 >= 0) {
        int prevPage = pages[currentPageIdx - 1];
        int lastHitIndex = m_searchRectMap[prevPage].size() - 1;
        jumpToHit(prevPage, lastHitIndex);
    }
}

void dodo::setupKeybinding(const QString &action,
                           const QString &key) noexcept
{
    auto it = m_actionMap.find(action);
    if (it == m_actionMap.end())
        return;

    QShortcut *shortcut = new QShortcut(QKeySequence(key), this);
    connect(shortcut, &QShortcut::activated, this, [=]() {
        it.value()();
    });

    m_shortcuts_map[action] = key;
}

void dodo::GoBackHistory() noexcept
{
    if (!m_page_history_list.isEmpty()) {
        int lastPage = m_page_history_list.takeLast(); // Pop last page
        m_suppressHistory = true;
        gotoPage(lastPage);
        m_suppressHistory = false;
    } else {
        qInfo("No page history available");
    }
}

void dodo::closeEvent(QCloseEvent *e)
{
    if (m_remember_last_visited && m_model->valid())
    {
        QSqlQuery q;
        q.prepare("INSERT OR REPLACE INTO last_visited(file_path, page_number) VALUES (?, ?)");
        q.addBindValue(m_filename);
        q.addBindValue(m_pageno);
        q.exec();
    }

    if (m_model->hasUnsavedChanges())
    {
        auto reply = QMessageBox::question(this, "Unsaved Changed",
                                           "There are unsaved changes in this document. Do you want to quit ?");
        if (reply == QMessageBox::StandardButton::No)
            return;
    }

    e->accept();
}

void dodo::scrollToNormalizedTop(const double &ntop) noexcept
{
    // 1. Get scene Y position of the top of the page
    qreal pageSceneY = m_pix_item->sceneBoundingRect().top();

    // 2. Get full height of the page
    qreal pageHeight = m_pix_item->boundingRect().height();

    // 3. Compute desired Y position in scene coords
    qreal targetSceneY = pageSceneY + ntop * pageHeight;

    // 4. Convert scene Y to viewport Y
    QPointF targetViewportPoint = m_gview->mapFromScene(QPointF(0, targetSceneY));

    // 5. Compute the scroll position required
    int scrollPos = m_gview->verticalScrollBar()->value() + targetViewportPoint.y();

    m_gview->verticalScrollBar()->setValue(scrollPos);

}

void dodo::TableOfContents() noexcept
{
    if (!m_owidget)
    {
        m_owidget = new OutlineWidget(m_model->clonedContext(), this);
        connect(m_owidget, &OutlineWidget::jumpToPageRequested, this, [=](int pageno) {
            gotoPage(pageno);
        });
    }

    if (!m_owidget->hasOutline())
    {
        fz_outline *outline = m_model->getOutline();
        if (!outline)
        {
            QMessageBox::information(this, "Outline", "Document does not have outline information");
            return;
        }
        m_owidget->setOutline(outline);
    }
    m_owidget->open();
}

void dodo::ToggleHighlight() noexcept
{
    m_gview->setDrawingMode(!m_gview->drawingMode());
}

void dodo::SaveFile() noexcept
{
    m_model->save();
    auto pix = m_model->renderPage(m_pageno, m_scale_factor, m_rotation);
    renderPixmap(pix);
}

void dodo::VisitLinkKB() noexcept
{
    this->installEventFilter(this);
    m_model->visitLinkKB(m_pageno, m_scale_factor);
    m_linkHintMode = true;
    m_link_hint_map = m_model->hintToLinkMap();
}

void dodo::CopyLinkKB() noexcept
{
    m_model->copyLinkKB(m_pageno);
}

bool dodo::eventFilter(QObject *obj, QEvent *event)
{
    if (m_linkHintMode)
    {
        if (m_link_hint_map.isEmpty())
        {
            m_linkHintMode = false;
            m_currentHintInput.clear();
            qWarning() << "No links found to show hints";
            return true;
        }
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape)
            {
                m_linkHintMode = false;
                m_currentHintInput.clear();
                m_model->clearKBHintsOverlay();
                this->removeEventFilter(this);
                return true;
            } else if (keyEvent->key() == Qt::Key_Backspace)
            {
                m_currentHintInput.removeLast();
                return true;
            }

            m_currentHintInput += keyEvent->text();
            if (m_link_hint_map.contains(m_currentHintInput))
            {
                Model::LinkInfo info = m_link_hint_map[m_currentHintInput];
                m_model->followLink(info);
                m_linkHintMode = false;
                m_currentHintInput.clear();
                m_link_hint_map.clear();
                m_model->clearKBHintsOverlay();
                this->removeEventFilter(this);
                return true;
            }
            keyEvent->accept();
            return true;
        }

        if (event->type() == QEvent::ShortcutOverride)
        {
            QShortcutEvent *stEvent = static_cast<QShortcutEvent *>(event);
            stEvent->accept();
            return true;
        }
    }

    // Let other events pass through
    return QObject::eventFilter(obj, event);
}

void dodo::GotoPage() noexcept
{
    int pageno = QInputDialog::getInt(this, "Goto Page",
                                      QString("Enter page number ({} to {})").arg(0, m_total_pages),
                                      0, 1, m_total_pages);
    gotoPage(pageno - 1);
}

bool dodo::hasUpperCase(const QString &text) noexcept
{
    for (const auto &c : text)
    if (c.isUpper())
        return true;

    return false;
}

void dodo::FileProperties() noexcept
{
    if (!m_propsWidget)
    {
        m_propsWidget = new PropertiesWidget(this);
        auto props = m_model->extractPDFProperties();
        m_propsWidget->setProperties(props);
    }
    m_propsWidget->exec();
}

// Puts the viewport to the top of the page
void dodo::TopOfThePage() noexcept
{
    auto vscrollbar = m_gview->verticalScrollBar();
    vscrollbar->setValue(0);
}

void dodo::ToggleFullscreen() noexcept
{
    if (this->isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

void dodo::InvertColor() noexcept
{
    m_model->invertColor();
    renderPage(m_pageno, true);
}

void dodo::ToggleAutoResize() noexcept
{
    m_auto_resize = !m_auto_resize;
}

void dodo::resizeEvent(QResizeEvent *e)
{
    if (m_auto_resize)
    {
        switch(m_fit_mode)
        {
            case FitMode::Height:
                FitHeight();
                break;

            case FitMode::Width:
                FitWidth();
                break;

            case FitMode::Window:
                FitWindow();
                break;
        }
    }

    e->accept();
}

void dodo::TogglePanel() noexcept
{
    bool shown = !m_panel->isHidden();
    m_panel->setHidden(shown);
    m_actionTogglePanel->setChecked(!shown);
}

void dodo::ToggleMenubar() noexcept
{
    bool shown = !m_menuBar->isHidden();
    m_menuBar->setHidden(shown);
    m_actionToggleMenubar->setChecked(!shown);
}

void dodo::ShowAbout() noexcept
{
    AboutDialog *abw = new AboutDialog(this);
    abw->setAppInfo("v0.2.0", "A fast, unintrusive pdf reader");
    abw->exec();
}

void dodo::readArgsParser(argparse::ArgumentParser &argparser) noexcept
{
    if (argparser["--version"] == true)
    {
        qInfo() << "dodo version: " << __DODO_VERSION;
        exit(0);
    }

    try
    {
        auto file = argparser.get<std::vector<std::string>>("files");
        if (!file.empty())
        {
            this->construct();
            openFile(QString::fromStdString(file[0]));
        }
    } catch (...)
    {
        this->construct();
    }
}

QRectF dodo::fzQuadToQRect(const fz_quad &q) noexcept
{
    fz_quad tq = fz_transform_quad(q, m_model->transform());

    return QRectF(
        tq.ul.x * m_inv_dpr,
        tq.ul.y * m_inv_dpr,
        (tq.ur.x - tq.ul.x) * m_inv_dpr,
        (tq.ll.y - tq.ul.y) * m_inv_dpr
    );
}

