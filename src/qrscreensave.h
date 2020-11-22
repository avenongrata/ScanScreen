#pragma once
#include <iostream>

#define PROGRAM_NAME "ScanScreen"

class QrScreenSave {
private:
    std::string mScreenPath;
    std::string mScreenFileName;

    std::string mGetFileName();

public:
    QrScreenSave();
    QrScreenSave(std::string);
    std::string takeScreen(void);

};
