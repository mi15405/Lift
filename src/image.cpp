#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <assert.h>
#include "image.h"

Image::Image(unsigned int w, unsigned int h)
    : width(w), height(h)
{
    if (width == 0 || height == 0)
        pixels = nullptr;
    else
    {
        /* 
         * Alocira se po 3 bajta za svaki piksel 
         * Po jedan bajt za svaku komponentu boje
         */
        pixels = new char[3 * width * height];

        /* Proverava da li je alokacija memorije uspesna */
        assert(pixels != nullptr);
    }
}

Image::~Image()
{
    /* 
     * Oslobadja se memorija prilikom unistavanja objekta, ukoliko je ucitana
     * slika
     */
    if (pixels != nullptr)
        delete[] pixels;
}

void Image::read(const char* filename)
{
    /* Ukoliko je slika ucitana, brise se */
    if (pixels != nullptr)
        delete[] pixels;

    pixels = nullptr;

    /* Otvaram fajl za citanje u binarnom rezimu */
    std::ifstream file(filename, std::ifstream::binary);

    if (!file)
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return;
    }

    /* Postavljam velicinu zaglavlja */
    unsigned int header_size = 54;

    /* Alociram bafer za ucitavanje podataka */
    char* buffer = new char [header_size];

    /* Ucitavam zaglavlje fajla u bafer */
    file.read(buffer, header_size);

    /* Proveravam da li je citanje uspesno */
    if (!file)
    {
        std::cerr << "Error while reading from file: " << filename << std::endl;
        file.close();
        return;
    }

    /* 
     * Polja koja su mi potrebna su sirina i visina slike kao i broj bitova po
     * pikselu. Posle kratkog sabiranja dolazim do sledecih pozicija:
     * width --> 18
     * height --> 22
     * bitcount --> 28
     */
    unsigned int width_pos = 18, height_pos = 22, bitcount_pos = 28;

    /* Kastujem char* u void* i dobijam pokazivac na deo memorije koji mi treba */
    void* p_width = static_cast<void*> (&buffer[width_pos]);
    void* p_height = static_cast<void*> (&buffer[height_pos]);
    
    /* Kastujem void* u unsigned int* i odmah ga dereferenciram */
    width = *static_cast<unsigned int*> (p_width);
    height = *static_cast<unsigned int*> (p_height);
    
    /* Isto radim i za broj bitova */
    void* p_bitcount = static_cast<void*> (&buffer[bitcount_pos]);
    unsigned short bitcount = *static_cast<unsigned short*> (p_bitcount);

    /* Oslobadjam alociranu memoriju */
    delete[] buffer;
    buffer = nullptr;

    
    /* Alociram memoriju za piksele u zavisnosti od broja bitova po pikselu */
    if (bitcount == 24)
        pixels = new char[3 * width * height];
    else if (bitcount == 32)
        pixels = new char[4 * width * height];
    else
    {
        std::cerr << "bitcount = " << bitcount << std::endl;
        std::cerr << "Image::read():"; 
        std::cerr << "Podrzane slike sa 32 ili 24 bita informacija po pixelu";
        exit(EXIT_FAILURE);
    }

    /* Proveravam da li je uspesno alocirana memorija */
    assert(pixels != nullptr);
    
    if (bitcount == 24)
        for (unsigned int i = 0; i < width * height; i++)
        {
            char bgr[3];

            /* Ucitavam piksele: blue, green, red */
            file.read(bgr, 3);
          
            /* Postavljam ucitane piksele */
            pixels[3 * i] = bgr[2];      /* red */
            pixels[3 * i + 1] = bgr[1];  /* green */
            pixels[3 * i + 2] = bgr[0];  /* blue */
        } 
    else if (bitcount == 32)
        for (unsigned int i = 0; i < width * height; i++)
        {
            char bgra[4];

            /* Ucitavam piksele: blue, green, red, alpha */
            file.read(bgra, 4);

            pixels[4 * i] = bgra[2];      /* red */
            pixels[4 * i + 1] = bgra[1];  /* green */
            pixels[4 * i + 2] = bgra[0];  /* blue */
            pixels[4 * i + 3] = bgra[3];  /* alpha */
        }
    
    /* Zatvaram fajl */
    file.close();
}

/* Geteri */
int Image::get_width(void) const { return width; }

int Image::get_height(void) const { return height; }

const char* Image::get_pixels(void) const { return pixels; }
