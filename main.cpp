/*
 * File: main.cpp
 * --------------
 * Blank C++ project configured to use Stanford cslib and Qt
 */

#include "console.h"
#include "simpio.h"
#include <iostream>
#include "image.h"
#include "grid.h"
#include "linemerge.h"
using namespace std;


int main() {
    image altered(0, 0);
    altered.read("work\\3image.bmp");
    Grid<int> grey = altered.getColorsGreyscale();

    altered.setColorsGreyscale(grey);
    altered.exportBmp("work\\3before.bmp");

    grey = photoToBmp(grey);
    altered.setColorsGreyscale(grey);
    altered.exportBmp("work\\3during.bmp");

    grey = mergeLines(grey, 1, 2, false, true);
    altered.setColorsGreyscale(grey);
    altered.exportBmp("work\\3after.bmp");

    return 0;
}
