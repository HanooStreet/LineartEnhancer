#include "image.h"
#include <iostream>
#include <fstream>
#include "grid.h"

//all code in image.cpp is by from this youtube video https://www.youtube.com/watch?v=NcEE5xmpgQ0&ab_channel=DesignedbyHugo with some modifications by me to utilize the stanford library
//with the exception of getColorsGreyscale and setColorsGreyscale, i created those myself (luminosity formula excluded)

Color::Color()
    : r(0), g(0), b(0)
{
}

Color::Color(float r, float g, float b)
    : r(r), g(g), b(b)
{
}

Color::~Color()
{
}

image::image(int width, int height)
    : m_width(width), m_height(height), m_colors(Vector<Color>(height * width))
{
}

image::~image()
{
}

Color image::getColor(int x, int y) const {
    return m_colors[y * m_width + x];
}

void image::setColor(const Color& color, int x, int y) {
    m_colors[y * m_width + x].r = color.r;
    m_colors[y * m_width + x].g = color.g;
    m_colors[y * m_width + x].b = color.b;
}

void image::read(const char* path) {
    std::ifstream f;
    f.open(path, std::ios::in | std::ios::binary);

    if (!f.is_open()) {
        std::cout << "invalid file. ";
        return;
    }

    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize];
    f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

    if (fileHeader[0] != 'B' || fileHeader[1] !='M') {
        std::cout << "file is not bitmap. ";
        f.close();
        return;
    }

    unsigned char informationHeader[informationHeaderSize];
    f.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

    int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
    m_width = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
    m_height = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

    int resizeSize = m_width * m_height;

    Vector<Color> m_colorsNew;
    if (m_colors.size() < resizeSize) {
        for (Color c : m_colors) {
            m_colorsNew.add(c);
        }
        for (int i = 0; i < resizeSize - m_colors.size(); i++) {
            m_colorsNew.add(Color(0,0,0));
        }
    } else {
        for (int i = 0; i < resizeSize; i++) {
            m_colorsNew.add(m_colors.get(i));
        }
    }
    m_colors = m_colorsNew;

    const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

    for (int y = 0; y < m_height; y++) {
        for(int x = 0; x < m_width; x++) {
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color), 3);

            m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
            m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
            m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;
        }
        f.ignore(paddingAmount);
    }

    f.close();
    std::cout << "file has been read! ";
}

Grid<int> image::getColorsGreyscale() {
    Grid<int> colors(m_width, m_height);
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            Color c = m_colors[y * m_width + x];
            int c2 = (1-c.r) * 255 * 0.3 + (1-c.g) * 255 * 0.59 + (1-c.b) * 255 * 0.11;
            if (c2 == 0) {
                c2 = 1;
            }
            colors.set(GridLocation(x, y), c2);
        }
    }
    return colors;
}

void image::setColorsGreyscale(Grid<int> colors) {
    Vector<Color> newColors;
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            int pixel = colors.get(GridLocation(x, y));
            newColors.add(Color(pixel, pixel, pixel));
        }
    }
    m_colors = newColors;
}

void image::exportBmp(const char* path) const {
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);

    if (!f.is_open()) {
        std::cout << "invalid file. ";
        return;
    }

    unsigned char bmpPad[3] = {0, 0, 0};
    const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;
    const int fileSize = fileHeaderSize + informationHeaderSize + m_width * m_height * 3 + paddingAmount * m_width;

    unsigned char fileHeader[fileHeaderSize];

    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;

    fileHeader[6] = 0;
    fileHeader[7] = 0;

    fileHeader[8] = 0;
    fileHeader[9] = 0;

    fileHeader[10] = fileHeaderSize + informationHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    unsigned char informationHeader[informationHeaderSize];

    informationHeader[0] = informationHeaderSize;
    informationHeader[1] = 0;
    informationHeader[2] = 0;
    informationHeader[3] = 0;

    informationHeader[4] = m_width;
    informationHeader[5] = m_width >> 8;
    informationHeader[6] = m_width >> 16;
    informationHeader[7] = m_width >> 24;

    informationHeader[8] = m_height;
    informationHeader[9] = m_height >> 8;
    informationHeader[10] = m_height >> 16;
    informationHeader[11] = m_height >> 24;

    informationHeader[12] = 1;
    informationHeader[13] = 0;

    informationHeader[14] = 24;

    for (int i = 15; i <= 39; i++) {
        informationHeader[i] = 0;
    }

    f.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
    f.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            unsigned char r = getColor(x, y).r * 255.0f;
            unsigned char g = getColor(x, y).g * 255.0f;
            unsigned char b = getColor(x, y).b * 255.0f;

            unsigned char color[]= {b, g, r};

            f.write(reinterpret_cast<char*>(color), 3);
        }
        f.write(reinterpret_cast<char*>(bmpPad), paddingAmount);
    }

    f.close();
    std::cout << "bmp made! ";
}
