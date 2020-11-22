#include "qrpicture.h"
#include <cstring>
#include <iostream>
#include <QPainter>

QrPicture::QrPicture() {}

QrPicture::QrPicture(std::string str, qrcodegen::QrCode::Ecc ecc)
{
    mUrl = str;
    mQr0 = qrcodegen::QrCode::encodeText(str.c_str(), ecc);
}

QPixmap & QrPicture::getQrPixMap(QPixmap &map, int height, int width)
{
    map = {height, width};
    QPainter painter(&map);
    mPaintQr(painter, QSize(height,width));

    return map;
}

void QrPicture::mPaintQr(QPainter &painter, const QSize sz, QColor fg)
{
    const int s = mQr0.getSize() > 0 ? mQr0.getSize() : 1;
    const double w = sz.width();
    const double h = sz.height();
    const double aspect = w/h;
    const double size = ((aspect > 1.0) ? h : w);
    const double scale = size/(s+2);
    double rx1, ry1;

    // Setting the background color....
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    for(int y=0; y<size; y++) {
        for(int x=0; x<size; x++) {
            rx1=x*scale, ry1=y*scale;
            QRectF r(rx1, ry1, scale, scale);
            painter.drawRects(&r,1);
        }
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(fg);
    for(int y=0; y<s; y++) {
        for(int x=0; x<s; x++) {
            const int color = mQr0.getModule(x, y);  // 0 for white, 1 for black
            if(0x0!=color) {
                const double rx1=(x+1) * scale, ry1=(y+1) * scale;
                QRectF r(rx1, ry1, scale, scale);
                painter.drawRects(&r,1);
            }
        }
    }

}

std::string QrPicture::getUrl()
{
    return mUrl;
}

