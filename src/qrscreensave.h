#pragma once

#include <iostream>

//-----------------------------------------------------------------------------

#define PROGRAM_NAME "ScanScreen"

//-----------------------------------------------------------------------------

class QrScreenSave
{
public:
    QrScreenSave();
    QrScreenSave(std::string);

    ~QrScreenSave() {}

    //-------------------------------------------------------------------------

    std::string takeScreen(void);

private:
    //-------------------------------------------------------------------------

    std::string mScreenPath;
    std::string mScreenFileName;

    //-------------------------------------------------------------------------

    std::string mGetFileName();
};
