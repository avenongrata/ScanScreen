#pragma once
#include <string>
#include "QrCode.h"
#include <QPainter>

class QrPicture
{
private:
    qrcodegen::QrCode mQr0;
    void mPaintQr(QPainter &, const QSize, QColor fg=Qt::black);
    std::string mUrl;

public:
    QrPicture();
    QrPicture(std::string, qrcodegen::QrCode::Ecc ecc=qrcodegen::QrCode::Ecc::MEDIUM);
    QPixmap & getQrPixMap(QPixmap &map, int height=250, int width=250);
    std::string getUrl();
};


