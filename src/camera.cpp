#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include "camera.h"

Camera::Camera(double x, double y, double z)
{
    /* Inicijalizacij pocetnih vrednosti vektora */
    get_pos().set(x, y, z);
    direction.set(-x, -y, -z);
    center.set(0, 0, 0);
    up_vector.set(0, 1, 0);

    /* Brzina kretanja i rotacije */
    move_speed = 10.0f;
    rotation_speed = 0.01f;

    /* 
     * Podrazumevano kamera moze da se kontrolise, ali je to moguce samo u modu
     * za debagovanje
     */
    free_look = true;

    /* Pocetne vrednosti polarnih koordinata pogleda */
    theta = 0;
    phi = 0;
}

void Camera::fixate_look(const Point3D& look) 
{ 
    /* Uperuje kameru u datu tacku */
    center.set(look); 

    /* Oduzima kontrolu nad kamerom */
    free_look = false;
}

void Camera::turn_on(void)
{
    Point3D position;

    if (free_look)
    {
        /* Ukoliko se kamera kontrolise, azuriraju se pogled i pozicija */
        update_view();
        position = get_pos();
    }
    else if (is_following_target)
    {
        /* Ukoliko kamera prati objekat, tacka koja se gleda se postavlja na
         * poziciju objekta, kao i pozicija kamere */
        center = target->get_pos();
        position = target->get_pos(); 

        /* Dodaje se razdaljina izmedju objekta i kamere */
        position.add(offset);
    }
    
    /* Prolsedjuju se vrednosti funkciji gluLookAt */
    gluLookAt(
        position.get_x(), position.get_y(), position.get_z(),
        center.get_x(), center.get_y(), center.get_z(),
        up_vector.get_x(), up_vector.get_y(), up_vector.get_z()
    );
}

void Camera::update_view(void)
{
    /* 
     * Prevodimo koordinate iz polarnog u dekartov sistem
     * i dobijamo pravac tacke koju gledamo 
     * */
    direction.set_x(cos(phi) * sin(theta));
    direction.set_y(sin(phi));
    direction.set_z(cos(phi) * cos(theta));

    /* 
     * Vektor pod uglom od 90 stepeni u odnosu na projekciju 
     * vektora pravca na xz ravan 
     */
    right_vector.set(sin(theta - M_PI/2), 0, cos(theta - M_PI/2));

    /* Vektor na gore, dobija se vektorskim proizvodom */
    up_vector.set_cross(direction, right_vector);

    /* Povecavamo intenzitet vektora */
    direction.mult(move_speed);
    right_vector.mult(move_speed);

    /* 
     * Dodajemo poziciju kamere na pravac tacke 
     * i dobijamo tacku koju gledamo 
     */
    center.set(get_pos());
    center.add(direction);
}

void Camera::add_theta(double delta_theta)
{
    /* Povecava se ugao */
    theta += delta_theta;

    /* 
     * Ukoliko predje preko punog kruga, ili ode u minus, oduzima se ili dodaje
     * pun krug 
     */
    if (theta > 2 * M_PI)
        theta -= 2 * M_PI;
    else if (theta < 0)
        theta += 2 * M_PI;
}

void Camera::add_phi(double delta_phi)
{
    /* Povecava se ugao */
    phi += delta_phi;

    /* 
     * Ukoliko predje preko pola kruga, ili ode u minus, oduzima se ili dodaje
     * pola kruga
     */
    if (phi > M_PI / 2)
        phi = M_PI / 2;
    else if (phi < -M_PI / 2)
        phi = -M_PI / 2;
}

void Camera::move_forward(void)
{
    /* Postavlja se brzina kretanja na pravac u kom se gleda */
    set_vel(direction);
}

void Camera::move_backward(void)
{
    /* Postavlja se brzina kretanja na pravac gledanja, u suprotnom smeru */
    direction.mult(-1);
    set_vel(direction);
    direction.mult(-1);
}

void Camera::move_right(void)
{
    /* Postavlja se brzina kretanja na pravac u desno */
    set_vel(right_vector);
}

void Camera::move_left(void)
{
    /* Postavlja se brzina kretanja na pravac u desno, u suprotnom smeru */
    right_vector.mult(-1);
    set_vel(right_vector);
    right_vector.mult(-1);
}

void Camera::move_up(void) 
{
    /* Postavlja se Y komponenta brzine, ostale se postavljaju na 0 */
    set_vel(0, move_speed, 0);
}

void Camera::move_down(void) 
{
    /* Postavlja se Y komponenta brzine, ostale se postavljaju na 0 */
    set_vel(0, -move_speed, 0);
}

void Camera::slow_down(void)
{
    /* Polovi se intenzitet vektora kretanja */
    get_vel_reference().mult(0.5);

    /* Ukoliko je intenzitet previse mali, postavlja se na nulu */
    if (get_vel().get_intensity() < 0.01)
        reset_vel();
}

void Camera::stop_moving(void)
{
    /* Postavlja brzinu na nulu */
    reset_vel();
}

void Camera::set_follow_target(
    Movable* follow, 
    float offset_x, 
    float offset_y, 
    float offset_z)
{
    /* Gasi se kontrola nad kamerom */
    free_look = false;

    /* Postavlja se fleg da kamera prati objekat */
    is_following_target = true;

    /* Postavlja se objekat koji se prati */
    target = follow;

    /* Postavlja se udaljenost kamere od objekta */
    offset.set(offset_x, offset_y, offset_z);
}

/* Seteri */
void Camera::set_free_look(bool look) { free_look = look; }

void Camera::set_move_speed(double ms) { move_speed = ms; }

void Camera::set_rotation_speed(double rs) { rotation_speed = rs; }

void Camera::set_direction(double x, double y, double z) { direction.set(x, y, z); }

void Camera::set_center(double x, double y, double z) { center.set(x, y, z); }

/* Geteri */
bool Camera::is_free_look(void) const { return free_look; }

double Camera::get_move_speed(void) const { return move_speed; }

double Camera::get_rotation_speed(void) const { return rotation_speed; }

Point3D& Camera::get_direction(void) { return direction; }

Point3D& Camera::get_center(void) { return center; }
