#pragma once

#include "grid.h"

int nearbyPixels (Grid<int>& colors, int x, int y, int box, int skip);
Grid<int> mergeLines (Grid<int> colors, double multipliers, int box, bool mask, bool picture);
int nearbyVoid (Grid<int>& colors, int x, int y);
Grid<int> photoToBmp (Grid<int> colors);
