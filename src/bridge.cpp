#include "bridge.h"
#include "gameController.h"

Bridge::Bridge()
{
    init();
}

void Bridge::init(void)
{
    /* Podesavanje dimenzija */
    length = 0.5f;
    height = 0.05f;
    width = 0.6f;

    /* Brzina rotacije lifta */
    rotation_speed = 160.0f;

    /* Pocetni ugao */
    angle = 90.0f;

    /* Podrazumevana je leva orijentacija */
    is_right = false;
}

void Bridge::reset(void)
{
    /* Postavljaju se pocetne vrednosti */
    rotation_speed = 160.0f;
    angle = 90.0f;
    width = 0.6f;
}

void Bridge::increase_rotation_speed(float factor)
{
    rotation_speed *= factor;
}

/* Vraca TRUE ako je most podignut */
bool Bridge::is_risen(void) const { return angle == 90.0f; }

/* Vraca TRUE ako je most spusten */
bool Bridge::is_lowered(void) const { return angle == 0.0f; }

void Bridge::rise(void)
{
    /* Ako je vec podignut, ne radi nista */
    if (is_risen())
        return;

    /* Povecavam ugao u zavisnosti od proteklog vremena */
    angle += rotation_speed * GameController::get_delta_time();

    /* Ne dozvojavam da ugao predje 90 stepeni */
    if (angle > 90)
        angle = 90;
}

void Bridge::lower(void)
{
    /* Ako je spusten, ne radi nista */
    if (is_lowered())
        return;

    /* Smanjujem ugao u zavisnosti od proteklog vremena */
    angle -= rotation_speed * GameController::get_delta_time();

    /* Ne dozvoljavam da ugao ode ispod 0 */
    if (angle < 0)
        angle = 0;
}

void Bridge::draw_object(void) const
{
    /* Iscrtavam lift, X koordinata zavisi od orijentacije */
    if (is_right)
        Drawable::draw_cuboid(length/2, 0, 0, length, height, width);
    else
        Drawable::draw_cuboid(-length/2, 0, 0, length, height, width);
}

void Bridge::update(void)
{
    /* Postavljam rotaciju u zavisnosti od orijentacije */
    if (is_right)
        set_rot(angle, 0, 0, 1);
    else
        set_rot(-angle, 0, 0, 1);
}

/* Seteri */
void Bridge::set_length(float l) { length = l; }

void Bridge::set_height(float h) { height = h; }

void Bridge::set_width(float w) { width = w; }

void Bridge::set_is_right(bool right) { is_right = right; }

void Bridge::set_rotation_speed(float speed) { rotation_speed = speed; }

/* Geteri */
bool Bridge::get_is_right(void) const { return is_right; }

float Bridge::get_width(void) const { return width; }
