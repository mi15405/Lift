#ifndef BRIDGE_H
#define BRIDGE_H

#include "drawable.h"

class Bridge: public Drawable
{
private:
    /* Dimenzije mosta */
    float length; 
    float height; 
    float width;

    /* Brzina rotacija */
    float rotation_speed;

    /* Trenutni ugao */
    float angle;

    /* Orijentacija, na koju stranu se spusta most */
    bool is_right;

public:
    Bridge();

    /* Inicijalizacija */
    void init(void);
    void reset(void);

    /* Animacija */
    void rise(void);
    void lower(void);
    bool is_risen(void) const;
    bool is_lowered(void) const;
    void increase_rotation_speed(float factor);

    /* Nasledjene metode */
    void draw_object(void) const override;
    void update(void);

    /* Seteri */
    void set_length(float l);
    void set_height(float h);
    void set_width(float w);
    void set_is_right(bool right);
    void set_rotation_speed(float speed);

    /* Geteri */
    bool get_is_right(void) const;
    float get_width(void) const;
};

#endif
