#include "elevator.h"
#include "gameController.h"
#include <cmath>
#include <iostream>

Elevator::Elevator()
{
    /* Inicijalizacija lifta */
    init(); 

    /* Inicijalizacija mostova */
    init_bridges();

    /* Podesavam parametre materijala */
    Material mat(
        0.0, 0.0, 0.2, 1,
        0.0, 0.0, 0.0, 1,
        0.0, 0.0, 0.5, 1, 
        2.0
    );

    /* Podesavam materijal */
    set_material(mat);
}

void Elevator::init(void)
{
    /* Inicijalizacija polja: */
    /* Dimenzije lifta */
    length = 1.0f;
    height = 0.05f;
    width = 0.4f;

    /* Inicijalizacija poena i nivoa */
    points = 0;
    level = 0;
    max_level = 8;

    /* Brzina transfera */
    load_speed = 2.0f;

    /* Brzina kretanja */
    move_speed = 4.0f;

    /* Minimalna brzina kretanja */
    min_move_speed = move_speed / 4;

    /* Ubrzanje */
    acc_speed = 0.3f;

    /* Faza kretanja */
    movement = STOP_MOVING;

    /* Faza transfera */
    transfer = TRANSFER_DONE;
    
    /* Kapacitet jednog reda (koliko loptica staje u njega) */
    row_capacity = 3;

    /* Dodaje se jedan (prvi) red za loptice */
    add_ball_row();
}

Elevator::~Elevator()
{
    /* 
     * Prilikom unistavanja objekta, unistavam sve redove loptica, jer su
     * dinamicki alocirani
     */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
        delete ball_rows[i];
}

void Elevator::init_bridges(void)
{
    /* Podesavam pozicije mostova tako da se nalaze na krajevima lifta */
    left_bridge.set_trans(-length/2, 0, 0);
    right_bridge.set_trans(length/2, 0, 0);

    /* Obelezavam desni most kao "desni" */
    right_bridge.set_is_right(true);
}

void Elevator::add_ball_row(void)
{
    /* Radijus loptica unutar reda */
    float ball_radius = 0.1;

    /* Razmak izmedju loptica unutar jednog reda */
    float offset = (length - row_capacity * 2 * ball_radius) / (row_capacity + 1);

    /* Kreiram novi red u dinamicki alociranoj memoriji */
    BallRow* ball_row = new BallRow(
        Point3D(-length/2, 0, 0),   /* Pocetak reda */
        Point3D(length/2, 0, 0),    /* Kraj reda */
        row_capacity,               /* Kapacitet reda */
        offset);                    /* Razmak */

    /* Postavljam brzinu kretanja loptica unutar reda */
    ball_row->set_speed(load_speed); 

    /* 
     * Postavljam centar koordinatnog sistema za koji je red loptica vezan
     * (u ovom slucaju to je centar lifta). To mi treba kada prebacujem loptice 
     * iz jednog reda u drugi, kako bih izracunao nove koordinate loptica
     * u odnosu na red u koji prelaze.
     */
    ball_row->set_parent(get_pos());

    /* Ubacujem novi red u vektor redova */
    ball_rows.push_back(ball_row);  
}

void Elevator::update(void) 
{
    /* Azuriraju se mostovi */
    left_bridge.update();
    right_bridge.update();

    /* Azurira se transfer, kretanje i trenutni nivo */
    update_transfer();    
    update_movement();
    update_level();

    /* Postavljam poziciju iscrtavanja lifta na poziciju lifta */
    Drawable::set_trans(get_pos());

    /* Azuriraju se redovi loptica i njihove referentne tacke */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        ball_rows[i]->set_parent(get_pos());
        ball_rows[i]->update();
    }
}

void Elevator::update_transfer(void)
{
    switch (transfer)
    {
        /* 
         * Ukoliko lift nije na istoj visina kao sprat sa kojim se vrsi
         * transfer, lift se poravnava sa spratom. Inace se prelazi na sledecu
         * fazu
         */
        case ALIGN_POSITION:
            if (!is_on_same_height(transfer_floor))
                align_with_floor(transfer_floor);
            else 
                transfer = LOWER_BRIDGE;
            break;

        /*
         * Ukoliko most koji je trenutno u funkciji (levi ako je utovar, desni
         * ako je istovar) nije spusten do kraja, spusta se most, u suprotnom
         * se prelazi na sledecu fazu u zavisnosti od mosta koji se spusta 
         */
        case LOWER_BRIDGE:
            if (!active_bridge->is_lowered())
                active_bridge->lower();
            else if (active_bridge == &left_bridge)
                transfer = LOADING;
            else
                transfer = UNLOADING;
            break;

        /* 
         * Loptice se prebacuju sa sprata u lift, jedna po jedna. Ukoliko nakon
         * transfera jedne loptice nema vise mesta ili je sprat prazan,
         * a pritom su sve loptice koje su prebacene usle u lift, prelazi se na
         * sledecu fazu
         */
        case LOADING:
            transfer_balls_inside();
            
            if ((!any_space_left() || transfer_floor->is_empty()) &&
                    all_balls_inside())
                transfer = LOADED;
            break;

        /*
         * Loptice se prebacuju iz lifta na sprat. Ako se sve prebacene loptice
         * presle na sprat, prelazi se na sledecu fazu.
         */
        case UNLOADING:
            transfer_balls_outside();

            if (transfer_floor->all_balls_inside()) 
                transfer = LOADED;
            break;

        /* Ukoliko most nije podignut, dize se. Inace je transfer zavrsen */
        case LOADED:
            if (!active_bridge->is_risen())
                active_bridge->rise();
            else
                transfer = TRANSFER_DONE;
            break;

        /* 
         * Ovde se nista ne desava, samo se cuva informacija da je zavrsen
         * transfer 
         */
        case TRANSFER_DONE:
            break;
    }
}

bool Elevator::is_on_same_height(Floor* floor)
{
    /* Proveravam da li je lift na rastojanju manjem od 0.01 od sprata */
    if (get_vertical_distance(floor) < 0.01)
    {
        /* 
         * Ako jeste, osiguravam se da su poravnati i postavljam ih na istu
         * visinu
         */
        set_pos_y(floor->get_trans_y());

        /* Postavljam brzinu lifta na 0 */
        reset_vel();
        return true;
    }
    return false;
}

void Elevator::align_with_floor(Floor* floor)
{
    /* U promenljivoj "target" upisujem poziciju lifta */
    Point3D target(get_pos());

    /* 
     * Postavljam Y koordinatu "target"-a na Y koordinatu lifta.
     * To je tacka do koje lift treba da stigne.
     */
    target.set_y(floor->get_trans_y());

    /* 
     * Postavljam brzinu poravnanja na pola brzine kretanja i pozivam metod
     * "move_to" kalse Movable koji pomera lift do "target"-a
     */
    float align_speed = move_speed / 2;
    move_to(target, align_speed);
}

bool Elevator::any_space_left(void) const
{
    /* Vraca TRUE ako lift nije popunjen */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        if (ball_rows[i]->space_left() > 0)
            return true;
    }
    return false;
}

bool Elevator::all_balls_inside(void)
{
    /* 
     * Vraca TRUE ako su sve loptice koje su prebacene u lift, zaista u liftu
     * (nisu na putu do lifta).
     */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        if (!ball_rows[i]->all_balls_in_row())
            return false;
    }
    return true;
}

void Elevator::transfer_balls_outside(void)
{
    /* Boja sprata sa kojim se vrsi transfer */
    Color floor_color = transfer_floor->get_color();

    /* Red loptica sprata sa kojim se vrsi transfer */
    BallRow& floor_row = transfer_floor->get_ball_row();

    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        BallRow* br = ball_rows[i];

        /* 
         * Ukoliko je red loptica lifta prazan i sve loptice, koje su do
         * sad prebacene na sprat, su presle na sprat, prelazi se na sledeci
         * red loptica
         */
        if (br->is_empty() && floor_row.all_balls_in_row())
            continue;

        /* 
         * Ukoliko red loptica nije spreman za transfer, napusta se funkcija
         */
        if (!br->transfer_ready())
            return;

        /* 
         * Ukoliko je boja loptice jednaka boji sprata, loptica se prebacuje na
         * sprat
         */
        if (br->get_first_ball()->get_color().equals(floor_color))
        {
            /* 
             * Prebacivanje loptica se vrsi izmedju redova loptica
             * i implementirano je u klasi BallRow
             */
            floor_row.transfer_ball_from(*br);

            /* Povecavaju se poeni zbog uspesno prebacene loptice */
            points++;
            return;
        }
    }
}

void Elevator::transfer_balls_inside(void)
{
    /* Sprat sa kog se prebacuju loptice u lift */
    BallRow& floor_row = transfer_floor->get_ball_row();
    
    /* Ukoliko je lift popunjen ili je sprat prazan, napusta se funkcija */
    if (!any_space_left() || transfer_floor->is_empty())
        return;
    
    /* 
     * Ukoliko je red loptica sprata spreman za transfer, bira se red loptica
     * lifta sa kojim se vrsi transfer
     */
    if (floor_row.transfer_ready())
    {
        BallRow* ball_row = pick_row_to_load();
        ball_row->transfer_ball_from(floor_row);
    }
}

BallRow* Elevator::pick_row_to_load()
{
    /* Boja loptice koja se prebacuje u lift */
    Color color = transfer_floor->get_ball_row().get_first_ball()->get_color();

    /* Red loptica lifta cija je poslednja loptica iste boje kao argument */
    BallRow* br = same_last_color_row(color);
    
    /* Ako ne postoji ovakav red, vraca se red sa najmanje loptica */
    if (br != nullptr)
        return br;
    else 
        return emptiest_row();
}

BallRow* Elevator::same_last_color_row(Color& ball_color)
{
    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        /* Razmatraju se samo redovi koji imaju loptice */
        if (ball_rows[i]->is_empty())
            continue;

        /* Boja poslednje loptice u redu */
        Color last_ball_color = ball_rows[i]->get_last_ball()->get_color();

        /* 
         * Ukoliko red nije popunjen i boja poslednje loptice je ista kao boja
         * prosledjena u argumentu, vraca se dati red loptica
         */
        if (ball_rows[i]->space_left() > 0 &&
            ball_color.equals(last_ball_color))
        {
            return ball_rows[i];
        }
    }

    return nullptr;
}

BallRow* Elevator::emptiest_row(void)
{
    /* Inicijalizuju se maksimalni preostali prostor i index */
    int max_space_left = 0;
    unsigned int index = 0;

    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        /* Preostali prostor u i-tom redu */
        int curr_space_left = ball_rows[i]->space_left();
        
        if (curr_space_left > max_space_left)
        {
            /* Azuriraju se vrednosti */
            max_space_left = curr_space_left;
            index = i;
        }
    }
    
    return ball_rows[index];
}

void Elevator::update_movement(void)
{
    /* Azurira se kretanje lifta u zavisnosti od trenutne faze kretanja */
    move();

    /* 
     * Movable::update pomera poziciju objekta (lifta) u zavisnosti od brzine
     * i proteklog vremena.
     * Detaljnije u klasi Movable
     */
    Movable::update();

    /* Ogranicava trenutnu brzinu i poziciju lifta */
    limit_velocity_and_position();
}

void Elevator::move(void)
{
    /* Ukoliko je transfer u toku, izlazi se iz funkcije */
    if (transfer != TRANSFER_DONE)
       return; 

    /* Referenca vektora brzine */
    Point3D& velocity = get_vel_reference();
    
    /* Y komponenta vektora brzine */
    float velocity_y = velocity.get_y();

    if (movement == MOVING_UP)
    {
        /* Ukoliko je brzina manja od minimalne, postavljam na minimalnu */
        if (velocity_y < min_move_speed)
           velocity.set(0.0f, min_move_speed, 0.0f); 

        /* Povecavam ubrzinu za ubrzanje "acc_speed" */
        velocity.add(0.0f, acc_speed, 0.0f);
    }
    else if (movement == MOVING_DOWN)
    {
        /* Isto samo u suprotnom smeru */
        if (velocity_y > -min_move_speed)
            velocity.set(0.0f, -min_move_speed, 0.0f);

        velocity.add(0.0f, -acc_speed, 0.0f);
    }
    else
    {
        /* Smanjuje se apsolutna vrednost brzine */
        slow_down();
    }
}

void Elevator::limit_velocity_and_position(void)
{
    /* 
     * Funkcija "limit_axis" prima osu kao prvi argument, 
     * a kao drugi i treci argument minimalnu i maksimalnu vrednost 
     * koje ogranicavaju vrednost date komponente(ose) tacke.
     * Ako je pozovemo nad tackom koja predstavlja brzinu, ogranicavamo brzinu
     * u pravcu ose (Y) na interval [-move_speed, move_speed].
     * Detaljnije u klasi Point3D
     */
    get_vel_reference().limit_axis('y', -move_speed, move_speed);

    /* Isto kao iznad, samo ovaj put ogranicavamo poziciju objekta */
    get_pos_reference().limit_axis('y', min_y, max_y);
}

void Elevator::slow_down(void)
{
    /* 
     * Apsolutna vrednost Y komponenta brzine, 
     * ostale komponente su ionako jednake nuli 
     */ 
    double vel_y = std::abs(get_vel().get_y());

    /* 
     * Minimalna brzina nakon koje necu vise smanjivati brzinu, vec cu je
     * postaviti na 0
     */
    double min_speed = 0.1f;

    if (vel_y > min_speed)
        get_vel_reference().mult(0.9f); /* Smanjujem brzinu kretanja */
    else
        reset_vel();                    /* Postavljam brzinu kretanja na nulu */
}

void Elevator::update_level(void)
{
    /* Nivo se ne azurira ako je vec na maksimumu */
    if (level >= max_level)
        return;

    /* 
     * Proverava da li je dostignut potreban broj poena za unapredjenje lifta.
     * Ta granica raste sa trenutnim nivoom lifta, po formuli ispod.
     * "get_total_capacity" vraca trenutni kapacitet lifta.
     */
    if (points > get_total_capacity() * (level + 3))
    {
        /* Povecava se nivo */
        level++; 
        
        /* 
         * Bira se tip unapredjenja na osnovu dostignutog nivoa.
         * Testiranjem sam dosao do toga da je bojle da se u pocetku povecava
         * kapacitet, a kasnije brzina lifta
         */
        if (level < 6)
            expand_size();
        else
            increase_speed();
    } 
}

Floor* Elevator::find_nearest_floor(std::vector<Floor*> floors)
{
    /* Postavljaju se vrednosti prvog sprata, rastojanje od lifta i indeks */
    float min_dist = get_vertical_distance(floors[0]);
    int nearest_index = 0;

    /* 
     * Prolazi se kroz sve spratove i vrednosti se azuriraju kada se nadje
     * blizi sprat. Optimizacija nije potrebna jer je mali broj spratova.
     * O(n) je sasvim okej.
     */
    for (unsigned int i = 1; i < floors.size(); i++)
    {
        float dist;
        if ((dist = get_vertical_distance(floors[i])) < min_dist)
        {
            min_dist = dist;
            nearest_index = i;
        }
    }
    return floors[nearest_index];
}

float Elevator::get_vertical_distance(Floor* floor)
{
    /* Vraca rastojanje izmedju prosledjenog sprata i lifta */
    float dist = get_pos().get_y() - floor->get_trans_y();
    return std::abs(dist);
}

void Elevator::cancel_transfer()
{
    /* Ukoliko je transfer vec zavrsen, ne desava se nista */
    if (transfer == TRANSFER_DONE)
        return;

    /* 
     * Zavrsava transfer loptica i zapocinje fazu podizanja mosta.
     * Funkcija ima jednu liniju, ali mislim da poboljsava citljivost koda, jer
     * je "cancel_transfer" intuitivnije od "transfer = LOADED".
     * Ovu funkciju igrac poziva pritiskanjem odgovarajuceg dugmeta, a korisna
     * je jer igrac moze da izabere da prebaci manje loptica nego sto ima mesta
     * u liftu (to je vec deo strategije i ostavlja se igracu da shvati sam)
     */
    transfer = LOADED;
}

void Elevator::start_transfer(std::vector<Floor*> floors)
{
    /* Ako prethodni transfer nije zavrsen, nema smisla zapocinjati novi */
    if (transfer != TRANSFER_DONE)
        return;

    /* Trazi se najblizi sprat od prosledjenih spratova */
    transfer_floor = find_nearest_floor(floors);
    
    /* 
     * Ako je najblizi sprat predaleko (vrednost 0.5 deluje razumno), ne
     * zapocinje se transfer
     */
    if (get_vertical_distance(transfer_floor) > 0.5)
        return;
    
    /* Proverava se da li je utovar ili istovar u pitanju */
    bool transfer_inside = transfer_floor->get_trans_x() < get_pos().get_x();

    if (transfer_inside)
    {
        /* Utovar loptica */
        if (transfer_floor->is_empty() || !any_space_left())
        {
            /* 
             * Ako je sprat za transfer prazan ili nema mesta u liftu,
             * ne zapocinje se transfer
             */
            return;
        }
        else
        {
            /* Inace se postavlja levi most kao aktivan most */
            active_bridge = &left_bridge;
        }
    }
    else
    {
        /* Istovar loptica */
        if (all_rows_empty()) /* Ako je lift prazan, ne zapocinje se transfer */
            return;
        else
            active_bridge = &right_bridge; /* Inace se postavlja desni most */
    }

    /* Faza transfera se postavlja na vertikalno poravnavanje sa spratom */
    transfer = ALIGN_POSITION;
}

bool Elevator::all_rows_empty(void) const
{
    /* Vraca TRUE ako su svi redovi loptica prazni */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
        if (!ball_rows[i]->is_empty())
            return false;
    return true;
}

void Elevator::expand_size(void)
{
    /* Vrednost za koju se povecava sirina lifta */
    float stretch = 0.3;

    /* Trenutna sirina mosta */
    float bridge_width = right_bridge.get_width();

    /* Postavljaju se nove sirine mostova, uvecane */
    right_bridge.set_width(bridge_width + stretch);
    left_bridge.set_width(bridge_width + stretch);

    /* Povecava se sirina lifta */
    width += stretch;

    /* Dodaje se jos jedan red loptica */
    add_ball_row();

    /* 
     * Zbog novog reda i novih dimenzija potrebno je poredjati redove
     * loptica tako da staju u lift 
     */
    arange_rows();
}

void Elevator::arange_rows()
{
    /* X koordinata pocetka reda */
    float start_x = -length / 2;

    /* X koordinata kraja reda */
    float end_x = -start_x;

    /* 
     * X i Y koordinate pocetka i kraja redova se ne menjaju, jer se dimenzija
     * lifta povecava samo u sirinu (menjaju se Z koordinate).
     * Racunam razmak izmedju dva reda, po Z osi.
     */
    float width_step = width / (ball_rows.size() + 1);

    for (unsigned int i = 0; i < ball_rows.size(); i++)
    {
        /* 
         * Pocevsi od jednog kraja lifta, pomeram Z koordinatu za width_step
         * i postavljam poziciju tekuceg reda.
         */
        float curr_width = -width/2 + width_step * (i+1);

        /* Postavljam pocetak reda */
        ball_rows[i]->set_start(Point3D(start_x, 0, curr_width));

        /* Postavljam kraj reda */
        ball_rows[i]->set_end(Point3D(end_x, 0, curr_width));
    }
}

void Elevator::increase_speed(void)
{
    /* Faktor ubrzanja */
    float factor = 1.2;

    move_speed *= factor;
    load_speed *= factor;
    acc_speed *= factor;

    /* Ubrzava se i rotacija mosta, tj koliko se most brzo spusta i podize */
    right_bridge.increase_rotation_speed(factor);
    left_bridge.increase_rotation_speed(factor);
}

void Elevator::draw_object(void) const
{
    glPushMatrix();
        /* 
         * Transliramo za pola visine na dole, kako bi centar lifta bio na
         * povrsini poda, a ne u sredini poda
         */
        glTranslatef(0, -height/2, 0);

        /* 
         * Aktiviram generaciju teksturnih koordinata, kako bih olaksao
         * mapiranje tekstura.
         * Nakon promene polozaja lifta, generisace se drugacije koordinate pa
         * se dobija efekat teksture koja se pomera. To sam naravno slucajno
         * otkrio i planiram da za sada ostane tako.
         * Detaljnije u klasi TextureManager.
         */
        TextureManager::enable_texture_generation();

        /* Ukljucujem teksturu za lift */
        TextureManager::enable_texture("elevator");

        /* 
         * Crta kvadar, prava tri argumenta su pozicija, druga tri dimenzije.
         * Detaljanije u klasi Drawable
         */ 
        Drawable::draw_cuboid(0, 0, 0, length, height, width);
        
        /* Dimenzije bocnih pregrada */
        double rail_width = 0.05, rail_height = 0.1;

        /* Aktiviranje tekstura */
        TextureManager::enable_texture("elevator_rails");

        /* Iscrtavanje bocnih pregrada */
        Drawable::draw_cuboid(
            0, 0, -width/2 - rail_width/2,
            length, rail_height, rail_width); 
        Drawable::draw_cuboid(
            0, 0, width/2 + rail_width/2,
            length, rail_height, rail_width); 

        /* Aktiviranje tekstura */
        TextureManager::enable_texture("bridge");

        /* Iscrtavanje mostova */
        left_bridge.draw();
        right_bridge.draw();

        /* Gasenje teksture */
        TextureManager::disable_texture();

        /* Gasenje generacije koordinata teksture */
        TextureManager::disable_texture_generation();

    glPopMatrix();

    /* Iscrtavanje loptica */
    for (unsigned int i = 0; i < ball_rows.size(); i++)
        ball_rows[i]->draw();
}

unsigned int Elevator::get_total_capacity(void) const
{
    /* Vraca trenutni kapacitet lifta */
    return ball_rows.size() * row_capacity;
}

void Elevator::reset(void)
{
    /* Brise sve redove loptica */
    while (!ball_rows.empty())
    {
        delete ball_rows.front();
        ball_rows.erase(ball_rows.begin());
    }

    /* Pokrece inicijalizaciju */
    init();

    /* Resetuje brzinu na 0 */
    reset_vel();

    /* Resetuje mostove na startna podesavanja */
    left_bridge.reset();
    right_bridge.reset();
}

/* Seteri */
void Elevator::set_movement(Movement m) { movement = m; }

void Elevator::set_max_y(float y) { max_y = y; }

void Elevator::set_min_y(float y) { min_y = y; }

unsigned int Elevator::get_points(void) const { return points; }
