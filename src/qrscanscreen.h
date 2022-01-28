#pragma once

#include <QMainWindow>

//-----------------------------------------------------------------------------

QT_BEGIN_NAMESPACE
namespace Ui
{
    class QrScanScreen;
}
QT_END_NAMESPACE

//-----------------------------------------------------------------------------

class QrScanScreen : public QMainWindow
{
    Q_OBJECT

    //-------------------------------------------------------------------------

public:
    QrScanScreen(QWidget *parent = nullptr);
    QrScanScreen(std::string, QWidget *parent = nullptr);
    ~QrScanScreen();

private slots:
    //-------------------------------------------------------------------------

private:
    //-------------------------------------------------------------------------

    Ui::QrScanScreen *ui;
};

