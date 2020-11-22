#include <windows.h>
#include "qrscanscreen.h"
#include "ui_qrscanscreen.h"
#include "qrpicture.h"
#include "QrCode.h"
#include <iostream>
#include <QClipboard>

QrScanScreen::QrScanScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QrScanScreen)
{
    ui->setupUi(this);
    //this->stackUnder(parentWidget());
}

QrScanScreen::~QrScanScreen()
{
    delete ui;
}

QrScanScreen::QrScanScreen(std::string str, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QrScanScreen)
{
    ui->setupUi(this);
    QrPicture code{str, qrcodegen::QrCode::Ecc::MEDIUM};
    QPixmap temp, &map = code.getQrPixMap(temp, 250, 250);  // is it legal ?
    ui->label_2->setPixmap(map);

    // add info about program and how to use it on screen
    std::string url = "Developed by nongrata\n"
                      "Program version 1.1\n\n"
                      "Scan Qr-code to visit site or\n"
                      "push button to get url/photo";
    ui->label->setText(QString::fromStdString(url));
    ui->label->setAlignment(Qt::AlignLeft);

    // add description about photo button
    ui->label_3->setText("Copy\n"
                         "screenshot\n"
                         "to\n"
                         "clipboard");
    ui->label_3->setAlignment(Qt::AlignCenter);

    // add description about url button
    ui->label_4->setText("Copy\n"
                         "url\n"
                         "to\n"
                         "clipboard");
    ui->label_4->setAlignment(Qt::AlignCenter);

    // copy screenshot to clipboard when user pushed the button
    // Make it later
    /*QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();
    clipboard->setText(newText);
    ui->pushButton->

    // copy url to clipboard when user pushed the button
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();
    clipboard->setText(newText);
    ui->pushButton->*/
}
