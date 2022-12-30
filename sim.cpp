#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <random>

enum Directions{
    north,
    south,
    east,
    west,
    all
};

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0.0, 1.0);

bool get_rand_bool(float chance = 0.5){
    return distribution(generator) < chance;
}

class Cell{
    protected:
    sf::Vector2f pos, area;
    sf::RenderWindow *window;
    public:
    Cell(sf::Vector2f pos, sf::Vector2f area, sf::RenderWindow *window): pos(pos), area(area), window(window) {}
};

class GridCell : public Cell{
    protected:
    sf::RectangleShape shape;
    int grid_height, grid_width;
    public:
    GridCell(sf::Vector2f pos, sf::Vector2f area, sf::RenderWindow *window, int grid_height, int grid_width): Cell(pos, area, window), grid_height(grid_height), grid_width(grid_width){}

    sf::Vector2f get_pos_in_direction(Directions in_direction){
        if (in_direction == all){
            return sf::Vector2f(-1,-1);
        }

        if (in_direction == north){
            return sf::Vector2f(pos.x, (pos.y > 0) ? (pos.y - 1) : grid_height - 1);
        }
        if (in_direction == east){
            return sf::Vector2f((pos.x + 1) < grid_width ? pos.x + 1 : 0, pos.y);
        }
        if (in_direction == south){
            return sf::Vector2f(pos.x, (pos.y + 1) < grid_height ? pos.y + 1: 0);
        }
        if (in_direction == west){
            return sf::Vector2f((pos.x > 0) ? (pos.x - 1) : grid_width - 1, pos.y);
        }
    }
    virtual void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, sf::Time elapsed){
        // Take actions here
    }
    virtual void draw(){
        // Draw Here
    }
};

class LeafCell: public GridCell{
    float density = 0;
    float total_time_alive = 0;
    float current_life_cycle = 0;

    int total_life_cycle = 5;
    int ripe_time = 2;
    int old_time = 4;
    int *new_growth_count;
    int propagation_level;

    sf::Color fill;
    sf::Color ripe;
    sf::Color old;
    public:
    LeafCell(sf::Vector2f pos, sf::Vector2f area, sf::RenderWindow *window, int grid_height, int grid_width, int propogation_level, int *new_growth_count) : GridCell(pos, area, window, grid_height, grid_width), ripe(0, 255, 0), old(218, 165, 32), propagation_level(propogation_level), new_growth_count(new_growth_count){
        shape.setPosition(sf::Vector2f(pos.x * area.x, pos.y * area.y));
        shape.setSize(area);
    }
    void draw(){
        shape.setFillColor(fill);
        window->draw(shape);
    }
    void propagate_leaves(GridCell **N, GridCell **E, GridCell **S, GridCell **W){
        if((*N) && (*E) && (*S) && (*W)) return;
        if (propagation_level <= 0) return;

        bool prop_n = get_rand_bool(0.25);
        bool prop_e = get_rand_bool(0.25);
        bool prop_s = get_rand_bool(0.25);
        bool prop_w = get_rand_bool(0.25);

        if(!(*N) && *new_growth_count > 0 && prop_n) {
            std::cout << "Propagating Leaf Cell North!" << std::endl;
            *N = new LeafCell(get_pos_in_direction(north), area, window, grid_width, grid_height, propagation_level - 1, new_growth_count);
            (*new_growth_count)--;
        }
        if(!(*E) && *new_growth_count > 0 && prop_e) {
            std::cout << "Propagating Leaf Cell East!" << std::endl;
            *E = new LeafCell(get_pos_in_direction(east), area, window, grid_width, grid_height, propagation_level - 1, new_growth_count);
            (*new_growth_count)--;
        }
        if(!(*S) && *new_growth_count > 0 && prop_s){
            std::cout << "Propagating Leaf Cell South!" << std::endl;
            *S = new LeafCell(get_pos_in_direction(south), area, window, grid_width, grid_height, propagation_level - 1, new_growth_count);
            (*new_growth_count)--;
        } 
        if(!(*W) && *new_growth_count > 0 && prop_w) {
            std::cout << "Propagating Leaf Cell West!" << std::endl;
            *W = new LeafCell(get_pos_in_direction(west), area, window, grid_width, grid_height, propagation_level - 1, new_growth_count);
            (*new_growth_count)--;
        }
    }
    void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, sf::Time elapsed){
        total_time_alive += elapsed.asSeconds();
        current_life_cycle = fmod(total_time_alive, total_life_cycle);

        if(current_life_cycle < ripe_time){
            if ((total_time_alive / total_life_cycle) >= 1){
                propagate_leaves(N, E, S, W);
            }
            density = current_life_cycle / ripe_time;
            fill = ripe;
        }else if (current_life_cycle > ripe_time && current_life_cycle < old_time){
            int total_steps = old_time - ripe_time;
            int current_step = current_life_cycle - ripe_time;

            fill.r = ripe.r + (current_step * ((old.r - ripe.r) / total_steps));
            fill.g = ripe.g + (current_step * ((old.g - ripe.g) / total_steps));
            fill.b = ripe.b + (current_step * ((old.b - ripe.b) / total_steps));
        }else {
            fill = old;
            density = (total_life_cycle - current_life_cycle) / (total_life_cycle - old_time);
        }
        
        if(density > 1){
            density = 1;
        }
        fill.a = 255 * density;
    }
};

class TrunkCell: public GridCell{
    float density = 0;
    float current_maturity = 0;
    float total_time_alive = 0;
    int maturity_time = 5;
    int current_season = 0;
    int past_season = 0;
    int season_allowable_new_growth = 1;
    public:
    using GridCell::GridCell;
    void draw(){
        float x_offset = (area.x - (area.x * density)) / 2;
        float y_offset = (area.y - (area.y * density)) / 2;


        shape.setPosition(sf::Vector2f((pos.x * area.x) + x_offset, (pos.y * area.y) + y_offset));
        shape.setSize(area * density);
        shape.setFillColor(sf::Color(0x96, 0x4B, 0));
        window->draw(shape);
    }
    void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, sf::Time elapsed){
        total_time_alive += elapsed.asSeconds();
        
        if (density > 1){
            if(!*N){
                std::cout << "Trunk Propagated North" << std::endl;
                *N = new LeafCell(get_pos_in_direction(north), area, window, grid_width, grid_height, 3, &season_allowable_new_growth);
            }
            if(!*E){
                std::cout << "Trunk Propagated East" << std::endl;
                *E = new LeafCell(get_pos_in_direction(east), area, window, grid_width, grid_height, 3, &season_allowable_new_growth);
            }
            if(!*S){
                std::cout << "Trunk Propagated South" << std::endl;
                *S = new LeafCell(get_pos_in_direction(south), area, window, grid_width, grid_height, 3, &season_allowable_new_growth);
            }
            if(!*W){
                std::cout << "Trunk Propagated West" << std::endl;
                *W = new LeafCell(get_pos_in_direction(west), area, window, grid_width, grid_height, 3, &season_allowable_new_growth);
            }
        }
        if (density < 1){
            density = current_maturity / maturity_time;
            current_maturity += elapsed.asSeconds();
        }else{
            density = 1;
        }
        current_season = total_time_alive / maturity_time;
        if (current_season != past_season){
            season_allowable_new_growth = 1;
            past_season = current_season;
        }
    }
};

class Grid{
    private:
    GridCell **grid;
    int grid_width, grid_height;
    sf::Vector2f block_area;
    sf::RenderWindow *window;
    TrunkCell *new_trunk;
    public:
    Grid(int window_p_width, int window_p_height, int grid_width, int grid_height, sf::RenderWindow *window) : grid_width(grid_width), grid_height(grid_height), window(window){
        // Initialize our grid
        grid = (GridCell **)malloc(sizeof(GridCell*) * (grid_height * grid_width));
        for (int i = 0; i < (grid_height * grid_width); i++) grid[i] = NULL;
        // Calculate our block area
        block_area.x = window_p_width / grid_width;
        block_area.y = window_p_height / grid_height;

        int trunk_x = 4;
        int trunk_y = 5;

        new_trunk = new TrunkCell(sf::Vector2f(trunk_x,trunk_y), block_area, window, grid_width, grid_height);
        grid[(trunk_y * grid_width) + trunk_x] = new_trunk;

    }

    void step(sf::Time elapsed){
        for (int y = 0; y < grid_height; y++){
            for (int x = 0; x < grid_width; x++){
                GridCell *curr_pointer = grid[(y * grid_width) + x];
                if(curr_pointer){
                    sf::Vector2f curr_n = curr_pointer->get_pos_in_direction(north);
                    sf::Vector2f curr_s = curr_pointer->get_pos_in_direction(south);
                    sf::Vector2f curr_e = curr_pointer->get_pos_in_direction(east);
                    sf::Vector2f curr_w = curr_pointer->get_pos_in_direction(west);

                    int n_index = (curr_n.y * grid_width) + curr_n.x;
                    int s_index = (curr_s.y * grid_width) + curr_s.x;
                    int e_index = (curr_e.y * grid_width) + curr_e.x;
                    int w_index = (curr_w.y * grid_width) + curr_w.x;

                    grid[(y * grid_width) + x]->step(&grid[n_index], &grid[e_index], &grid[s_index], &grid[w_index], elapsed);
                }
            }
        }
    }
    void draw(){
        int draw_count = 0;
        for (int y = 0; y < grid_height; y++){
            for (int x = 0; x < grid_width; x++){
                if(grid[(y * grid_width) + x]){
                    grid[(y * grid_width) + x]->draw();
                    draw_count++;
                }
            }
        }
    }
};


int main()
{
    int window_p_width = 800;
    int window_p_height = 600;
    generator.seed(time(0));

    // create the window
    sf::RenderWindow window(sf::VideoMode(window_p_width, window_p_height), "Tree Propogation 2D");
    Grid grid(window_p_width, window_p_height, 80, 60, &window);
    sf::Clock clock;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);
        grid.step(clock.restart());
        grid.draw();

        // end the current frame
        window.display();
    }

    return 0;
}