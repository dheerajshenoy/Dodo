#pragma once

#include <QRunnable>
#include <poppler/qt6/poppler-qt6.h>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <mupdf/fitz.h>

class RenderTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    RenderTask(fz_context *ctx, fz_document *doc, fz_colorspace *colorspace, int pageno, fz_matrix transform);
    void run() override;  // QRunnable

signals:
    void finished(int pageno, QImage image);

private:
    fz_context *m_ctx;
    fz_document *m_doc;
    fz_colorspace *m_colorspace;
    int m_pageno;
    fz_matrix m_transform;
    bool m_lowQuality;
};

