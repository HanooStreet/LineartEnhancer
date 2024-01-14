#include "linemerge.h"
#include "grid.h"
#include <math.h>
#include "image.h"
using namespace std;

//calculates and returns the amount of park areas near a given pixel
int nearbyPixels (Grid<int>& colors, int x, int y, int box, int skip, double multiplier) {
    int count = 0;
    float sum = 0;

    for (int y2 = y - box; y2 <= y + box; y2 += skip) {
        for (int x2 = x - box; x2 <= x + box; x2 += skip) {
            int distance = 1 - (max(abs(y2-y), abs(x2-x)) / box);
            sum += colors.get(GridLocation(x2, y2)) * pow(distance, 4);
        }
        count++;
    }
    return (pow(sum, multiplier)) / (count * count);
}

//increases line width of lines that are intersecting other lines
Grid<int> mergeLines (Grid<int> colors, double multiplier, int box, bool mask, bool picture) {

    int skip = box / 3;
    if (skip < 1) {
        skip = 1;
    }

    cout << endl << endl << "rows: " << colors.numCols() << endl;
    cout << "columns: " << colors.numRows() << endl;
    cout << "scanner dimensions: " << box*2 + 1 << " by " << box*2 + 1 << endl;
    cout << "scanner preciseness: " << skip << " pixel" << endl << endl;

    Grid<int> newColors(colors.numRows(), colors.numCols());

    const int sections = 60;
    for (int y3 = 0; y3 < (colors.numCols() - box*2)/sections; y3++) {
        for (int x3 = 0; x3 < (colors.numRows() - box*2)/sections; x3++) {
            double sum = -1;
            for (int a = 0; a < 8 && sum < 0.003; a++) {
                sum = 0;
                for (int y2 = 0; y2 < sections; y2++) {
                    for (int x2 = 0; x2 < sections; x2++) {
                        int x = x3*sections + x2 + box;
                        int y = y3*sections + y2 + box;
                        int pixel = nearbyPixels(colors, x, y, box, skip, multiplier + 0.1*a);
                        int first = colors.get(GridLocation(x, y));
                        if (!picture) {
                            if (first > pixel && mask) {
                                pixel = first;
                            } else if (pixel > 255) {
                                pixel = 255;
                                sum++;
                            } else if (pixel < 100) {
                                if (first < 100 && mask) {
                                    pixel = first;
                                } else {
                                    pixel = 1;
                                }
                            } else {
                                sum++;
                            }
                            newColors.set(GridLocation(x, y), pixel);
                        } else {
                            if (pixel < 2500) {
                                pixel = 1;
                            } else {
                                pixel = first;
                            }
                            newColors.set(GridLocation(x, y), pixel);
                        }
                    }
                }
                sum = sum / sections / sections;
            }
        }
    }

    /*
    for (int y = box; y < colors.numCols() - box; y++) {
        for (int x = box; x < colors.numRows() - box; x++) {
            int pixel = nearbyPixels(colors, x, y, box, skip, 1.3);
            int first = colors.get(GridLocation(x, y));
            if (first > pixel) {
                pixel = first;
            } else if (pixel > 255) {
                pixel = 255;
            } else if (pixel < 100) {
                if (first < 100) {
                    pixel = first;
                } else {
                    pixel = 1;
                }
            } else {
            }
            newColors.set(GridLocation(x, y), pixel);
        }
    } */
    return newColors;
}

int nearbyVoid (Grid<int>& colors, int x, int y) {

    int pixel1 = 0;
    int y2 = y - 4;
    for (y--; y > y2; y--) {
        pixel1 += colors.get(GridLocation(x, y));
    }

    int pixel2 = 0;
    y2 = y + 8;
    y += 4;
    for (y++; y < y2; y++) {
        pixel2 += colors.get(GridLocation(x, y));
    }

    if (pixel1 > pixel2) {
        return pixel1 - pixel2;
    }
    return pixel2 - pixel1;
}

Grid<int> photoToBmp (Grid<int> colors) {

    Grid<int> newColors(colors.numRows(), colors.numCols());

    for (int y = 4; y < colors.numCols() - 3; y++) {
        for (int x = 0; x < colors.numRows(); x++) {
            int pixel = nearbyVoid(colors, x, y);
            pixel = pow(pixel, 1.2);
            pixel *= colors.get(GridLocation(x, y));
            pixel = pow(pixel, 0.5);
            if (pixel > 255) {
                pixel = 255;
            } else if (pixel < 80) {
                pixel = 1;
            }
            newColors.set(GridLocation(x, y), pixel);
        }
    }

    return newColors;
}
