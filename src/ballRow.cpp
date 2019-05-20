#include <cstdlib>
#include <iostream>
#include "ballRow.h"
#include "movable.h"

BallRow::BallRow(
    const Point3D& start, 
    const Point3D& end, 
    unsigned int capacity, 
    float offset, 
    float speed)
        : start(start), 
          end(end), 
          capacity(capacity), 
          offset(offset), 
          speed(speed)
{
    /* Podrazumevani radijus loptica */
    ball_radius = 0.1f;
    
    /* 
     * Podrazumevana opcija je da se loptice ne unistavaju kada stignu do kraja
     * reda
     */
    destroy_at_end = false;

    /* Inicijalizacija pozicija */
    init_positions();
}

BallRow::~BallRow()
{
    /* Unistavaju se sve loptice */
    while (!balls.empty())
    {
        Ball* ball = pop_first_ball();
        delete ball;
    }
}

void BallRow::update(void)
{
    /* Azuriraju se brzine kretanja loptica */
    update_vel();

    /* Azuriraju se loptice */
    for (unsigned int i = 0; i < balls.size(); i++)
        balls[i]->update();

    /* 
     * Ukoliko je loptica stigla do kraja i fleg je podesen,
     * unistava se loptica
     */
    if (destroy_at_end && transfer_ready())
        destroy_first_ball();
}

void BallRow::destroy_first_ball(void)
{
    /* Unistava prvu lopticu u redu */
    Ball* ball = pop_first_ball();
    delete ball;
}

void BallRow::update_vel(void)
{
    /* Broj pozicije u redu */
    int pos_num = 0;

    for (unsigned int i = 0; i < balls.size(); i++)
    {
        Point3D target;

        if (!is_in_row(balls[i]))
        {
            /* 
             * Ukoliko loptica nije unutar reda, trebalo bi da ide do pocetne
             * pozcijie
             */
            target.set(start);
        }
        else 
        {
            /* 
             * Ukoliko je loptica unutar reda, dobjia svoju poziciju iz liste
             * svih pozicija
             */
            target.set(positions[pos_num++]);
        } 

        /* Pomera lopticu do zeljene pozicije datom brzinom */
        balls[i]->move_to(target, speed);
    }
}

void BallRow::add_ball(Ball* ball)
{
    /* Ukoliko ima mesta u redu, dodaje loptu u red */
    if (space_left() > 0)
        balls.push_back(ball);
}

void BallRow::spawn_ball(void)
{
    /* Ukoliko nema mesta u redu, loptica se ne stvara */
    if (space_left() <= 0)
        return;

    /* Generise se nasumicna boja */
    Color color;
    color.randomize();

    /* Kreira se nova loptica u dinamicki alociranoj memoriji */
    Ball* ball = new Ball(ball_radius, color);

    /* Postavlja se pozicija na pocetak */
    ball->set_pos(start);

    /* Postavlja se brzina loptice */
    ball->set_vel(speed, 0, 0);

    /* Dodaje se loptica u red loptica */
    add_ball(ball);
}

Ball* BallRow::pop_first_ball(void)
{
    /* Izbacuje prvu lopticu iz redu i vraca je kao povratnu vrednost */
    Ball* ball = balls.front();
    balls.erase(balls.begin());

    return ball;
}

Ball* BallRow::get_first_ball(void)
{
    /* Vraca prvu loptu */
    if (is_empty())
        return nullptr;
    else
        return balls.front();
}

Ball* BallRow::get_last_ball(void)
{
    /* Vraca poslednju loptu */
    if (is_empty())
        return nullptr;
    else
        return balls.back();
}

void BallRow::transfer_ball_from(BallRow& other)
{
    /* 
     * Ukoliko nema mesta u redu ili je red iz kog se prebacuju loptice prazan,
     * zavrsava se transfer loptica 
     */
    if (space_left() <= 0 || other.balls.size() <= 0)
        return;

    /* Uzima se loptica iz reda prosledjenog kao argument */
    Ball* ball = other.pop_first_ball();

    /* 
     * Pozicija loptice u odnosu na red loptica iz kog je uzeta (njegovu
     * referentnu tacku).
     */
    Point3D new_pos = ball->get_pos();

    /* 
     * Prebacujem koordinate uzete loptice u koordinatni sistem reda loptica
     * u koji ulazi (to je red loptica ovog objekta). Koristim referentne tacke
     * za izracunavanje.
     * Detaljnije u klasi Point3D
     */
    new_pos.change_coord_system(other.parent, parent);

    /* Postavljam nove koordinate uzete loptice */
    ball->set_pos(new_pos);

    /* Dodajem lopticu u red */
    add_ball(ball);
}

bool BallRow::is_in_row(Ball* ball) const
{
    /* 
     * Proverava da li je loptica unutar reda.
     * Posto se svi redovi pruzaju u pravcu X ose, dovoljno je da proverim
     * samo X koordinatu lopte.
     */
    double pos_x = ball->get_pos().get_x();

    if (pos_x >= start.get_x() && pos_x <= end.get_x())
        return true;
    return false;
}

bool BallRow::all_balls_in_row(void) const
{
    /* Proverava da li su sve loptice unutar reda */
    for (Ball* ball: balls)
        if (!is_in_row(ball))
            return false;
    return true;
}

bool BallRow::is_empty(void) const
{
    /* Proverava da li je red loptica prazan */
    return balls.empty();
}

bool BallRow::is_full(void) const
{
    /* Proverava da li je red loptica pun */
    return balls.size() >= capacity;
}

bool BallRow::transfer_ready(void) const
{
    /* 
     * Transfer je spreman ukoliko ima loptica u redu i prva loptica se nalazi
     * na svojoj poziciji (prva pozicija)
     */
    if (!is_empty() && !balls[0]->get_pos().is_different_from(positions[0]))
        return true;
    return false;
}

void BallRow::draw_object(void) const
{
    /* Iscrtava sve loptice */
    glPushMatrix();
        /*
         * Posto se pocetna i krajnja tacka reda nalaze u nivou poda,
         * transliram na gore za radijus loptice
         */
        glTranslatef(0, ball_radius, 0);

        /* Iscrtavam loptice */
        for (unsigned int i = 0; i < balls.size(); i++)
            balls[i]->draw();

    glPopMatrix();
}

void BallRow::set_capacity(unsigned int num) 
{ 
    capacity = num; 

    /* Nakon sto se kapacitet promeni, ponovo se racunaju pozicije */
    init_positions();
}


/* Vraca broj preostalih mesta u redu */
int BallRow::space_left(void) const { return capacity - balls.size(); }

void BallRow::init_positions(void)
{
    /* Ukoliko ima pozicija, brisu se sve stare pozicije */
    if (!positions.empty())
        positions.clear();

    /* Koordinata kraja reda */
    Point3D pos(end);

    /* X koordinata se pomera za radijus loptice i razmak */
    pos.add(-ball_radius - offset, 0, 0);

    /* Dodaje se prva pozicija */
    positions.push_back(pos);

    for (unsigned int i = 1; i < capacity; i++)
    {
        /* Sledeca pozcicija se racuna u odnosu na prethodnu */
        Point3D next_pos(positions[i-1]);

        /* Pomera se po X osi za precink loptice i razmak */
        next_pos.sub(ball_radius * 2 + offset, 0, 0);

        /* Ubacuje se pozicija u vektor svih pozicija */
        positions.push_back(next_pos);
    }
}

void BallRow::set_start(const Point3D& s) 
{ 
    start.set(s); 

    /* Nakon promene pocetka reda, ponovo se racunaju pozicije */
    init_positions();
}

void BallRow::set_end(const Point3D& e) 
{ 
    end.set(e); 

    /* Nakon promene kraja reda, ponovo se racunaju pozicije */
    init_positions();
}

void BallRow::set_start_end(const Point3D& s, const Point3D& e)
{
    start.set(s);
    end.set(e);

    /* Nakon promene pocetka i kraja reda, ponovo se racunaju pozicije */
    init_positions();
}

void BallRow::reset(void)
{
    /* Unistavaju se sve loptice */
    while (!balls.empty())
    {
        Ball* ball = pop_first_ball();
        delete ball;
    }
}

/* Seteri */
void BallRow::set_destroy_at_end(bool destroy) { destroy_at_end = destroy; }

void BallRow::set_parent(const Point3D& p) { parent.set(p); }

void BallRow::set_offset(float dist) { offset = dist; }

void BallRow::set_speed(float s) { speed = s; }

void BallRow::set_ball_radius(float radius) { ball_radius = radius; }

/* Geteri */
Point3D& BallRow::get_start(void) { return start; }

Point3D& BallRow::get_end(void) { return end; }

/* Debagovanje */
void BallRow::print_positions(void) const
{
    std::cout << "BallRow positions: " << std::endl;
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        std::cout << positions[i].get_x() << " "; 
        std::cout << positions[i].get_y() << " "; 
        std::cout << positions[i].get_z() << " "; 
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "start: ";
    std::cout << start.get_x() << " ";
    std::cout << start.get_y() << " ";
    std::cout << start.get_z() << " ";
    std::cout << std::endl;

    std::cout << "end: ";
    std::cout << end.get_x() << " ";
    std::cout << end.get_y() << " ";
    std::cout << end.get_z() << " ";
    std::cout << std::endl;
}
