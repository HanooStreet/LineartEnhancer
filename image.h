#ifndef IMAGE_H
#define IMAGE_H
#include "vector.h"
#include "grid.h"

//all code in image.cpp is by from this youtube video https://www.youtube.com/watch?v=NcEE5xmpgQ0&ab_channel=DesignedbyHugo with some modifications by me to utilize the stanford library
//with the exception of getColorsGreyscale and setColorsGreyscale, i created those myself (luminosity formula excluded)

struct Color {
    float r, g, b;

    Color();
    Color(float r, float g, float b);
    ~Color();

};

class image
{
public:
    image(int width, int height);
    ~image();

    Color getColor(int x, int y) const;
    void setColor(const Color& color, int x, int y);

    void read(const char* path);
    void exportBmp(const char* path) const;
    Grid<int> getColorsGreyscale();
    void setColorsGreyscale(Grid<int>);
private:
    int m_width;
    int m_height;
    Vector<Color> m_colors;
};

#endif
