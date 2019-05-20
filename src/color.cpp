#include <cstdlib>
#include <iostream>
#include <ctime>
#include "color.h"

Color::Color(Rgb col)
    : rgb(col)
{}

std::vector<std::string> Color::colors =
    {
        "red", "green", "blue", "yellow",
        "purple", "cyan", "gray"
    };

std::vector<std::string> Color::get_colors(void)
{
    return colors;
}

bool Color::equals(const Color& other)
{
    return rgb == other.rgb;
}

void Color::set_rgb_array(float* p_rgb, float intensity, float base)
{
    /* Postavlja se pocetna vrednost */
    p_rgb[0]= base; 
    p_rgb[1]= base; 
    p_rgb[2]= base; 

    /* Postavljaju se ostale vrednosti, u zavisnosti od boje */
    switch (rgb)
    {
        case RED:
            p_rgb[0] = intensity;
            break;
        case GREEN:
            p_rgb[1] = intensity;
            break;
        case BLUE:
            p_rgb[2] = intensity;
            break;
        case YELLOW:
            p_rgb[0] = intensity;
            p_rgb[1] = intensity;
            break;
        case PURPLE:
            p_rgb[0] = intensity - 0.2;
            p_rgb[1] = 0.2;
            p_rgb[2] = intensity - 0.2;
            break;
        case CYAN:
            p_rgb[0] = 0.2;
            p_rgb[1] = intensity;
            p_rgb[2] = intensity;
            break;
        case NO_COLOR:
            std::cerr << "NO COLOR SELECTED" << std::endl;
            break;
        default:
            std::cerr << "UNKNOWN COLOR" << std::endl;
            break;
    }
}

void Color::randomize(void)
{
    /* Uzima jednu od prvih 6 vrednosti */
    rgb = Rgb(rand() % 6);
}

std::string Color::get_color(void) const
{
    switch (rgb)
    {
        case RED:
            return "red";
        case GREEN:
            return "green";
        case BLUE:
            return "blue";
        case YELLOW:
            return "yellow";
        case PURPLE:
            return "purple";
        case CYAN:
            return "cyan";
        case NO_COLOR:
            return "gray";
        default:
            std::cerr << "UNKNOWN COLOR" << std::endl;
            return NULL;
    }
}

Rgb Color::get_rgb(void) const { return rgb; }
