#ifndef IMAGE_H
#define IMAGE_H

/* 
 * Ne koristim strukture pri citanju, vec sam izracunao pozicije koje su mi
 * potrebne (width, height i bitcount), te ucitavam samo njih iz bafera.
 */

/* Preuzeto sa vezbi :) */
typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offsetbits;
} BITMAPFILEHEADER;

/* Preuzeto sa vezbi :) */
typedef struct {
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short bitcount;
    unsigned int compression;
    unsigned int sizeimage;
    int xpelspermeter;
    int ypelspermeter;
    unsigned int colorsused;
    unsigned int colorsimportant;
} BITMAPINFOHEADER;

class Image
{
private:
    unsigned int width, height;
    char* pixels;

public:
    Image(unsigned int width = 0, unsigned int height = 0);
    ~Image();

    void read(const char* filename);

    /* Geteri */
    int get_width(void) const;
    int get_height(void) const;
    const char* get_pixels(void) const;
};

#endif
