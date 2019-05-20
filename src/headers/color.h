#ifndef COLOR_H
#define COLOR_H

#include <vector>
#include <string>

typedef enum 
{ 
    RED, 
    GREEN, 
    BLUE, 
    YELLOW, 
    PURPLE, 
    CYAN, 
    NO_COLOR 
} Rgb;

class Color
{
private:
    Rgb rgb;
    static std::vector<std::string> colors;

public:
    Color(Rgb color = NO_COLOR);

    void randomize(void);
    bool equals(const Color& other);
    void set_rgb_array(float* rgb, float intensity = 1.0f, float base_color = 0.0f);
    
    /* Geteri */
    static std::vector<std::string> get_colors(void);
    std::string get_color(void) const;
    Rgb get_rgb(void) const;
};

#endif
