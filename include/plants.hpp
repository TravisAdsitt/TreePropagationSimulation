#include "utils.hpp"

class SeedCell: public GridCell{
    public:
    using GridCell::GridCell;
};

class LeafCell: public GridCell{
    float density = 0;

    int current_life_cycle = 0;

    int total_life_cycle = 10;
    int ripe_time = 2;
    int old_time = 6;
    int *new_growth_count;
    int propagation_level;

    sf::Color fill;
    sf::Color ripe;
    sf::Color old;
    public:
    LeafCell(sf::Vector2f pos, sf::RenderWindow *window, int propogation_level, int *new_growth_count, uint32_t tick_born) : GridCell(pos, window, tick_born), ripe(0, 255, 0), old(218, 165, 32), propagation_level(propogation_level), new_growth_count(new_growth_count){
        shape.setPosition(sf::Vector2f(pos.x * area.x, pos.y * area.y));
        shape.setSize(area);
    }
    void draw(){
        shape.setFillColor(fill);
        window->draw(shape);
    }
    void propagate_leaves(GridCell **N, GridCell **E, GridCell **S, GridCell **W, int current_tick){
        if((*N) && (*E) && (*S) && (*W)) return;
        if (propagation_level <= 0) return;

        bool prop_n = get_rand_bool(0.25);
        bool prop_e = get_rand_bool(0.25);
        bool prop_s = get_rand_bool(0.25);
        bool prop_w = get_rand_bool(0.25);

        if(!(*N) && *new_growth_count > 0 && prop_n) {
            *N = new LeafCell(get_pos_in_direction(north), window, propagation_level - 1, new_growth_count, current_tick);
            (*new_growth_count)--;
        }
        if(!(*E) && *new_growth_count > 0 && prop_e) {
            *E = new LeafCell(get_pos_in_direction(east), window, propagation_level - 1, new_growth_count, current_tick);
            (*new_growth_count)--;
        }
        if(!(*S) && *new_growth_count > 0 && prop_s){
            *S = new LeafCell(get_pos_in_direction(south), window, propagation_level - 1, new_growth_count, current_tick);
            (*new_growth_count)--;
        } 
        if(!(*W) && *new_growth_count > 0 && prop_w) {
            *W = new LeafCell(get_pos_in_direction(west), window, propagation_level - 1, new_growth_count, current_tick);
            (*new_growth_count)--;
        }
    }

    void propagate_seed(GridCell **N, GridCell **E, GridCell **S, GridCell **W, int current_tick){
        if((*N) && (*E) && (*S) && (*W)) return;
        if (propagation_level > 0) return;

        bool prop_n = get_rand_bool(0.01);
        bool prop_e = get_rand_bool(0.01);
        bool prop_s = get_rand_bool(0.01);
        bool prop_w = get_rand_bool(0.01);

        if(!(*N) && *new_growth_count > 0 && prop_n) {
            *N = new SeedCell(get_pos_in_direction(north), window, current_tick);
            (*new_growth_count)--;
        }
        if(!(*E) && *new_growth_count > 0 && prop_e) {
            *E = new SeedCell(get_pos_in_direction(north), window, current_tick);
            (*new_growth_count)--;
        }
        if(!(*S) && *new_growth_count > 0 && prop_s){
            *S = new SeedCell(get_pos_in_direction(north), window, current_tick);
            (*new_growth_count)--;
        } 
        if(!(*W) && *new_growth_count > 0 && prop_w) {
            *W = new SeedCell(get_pos_in_direction(north), window, current_tick);
            (*new_growth_count)--;
        }
    }

    void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, int current_tick){
        current_life_cycle = (current_tick - tick_born) % total_life_cycle;

        if(current_life_cycle <= ripe_time){
            if (current_life_cycle == 0 && (current_tick - tick_born) > 0){
                propagate_leaves(N, E, S, W, current_tick);
                propagate_seed(N, E, S, W, current_tick);
            }
            density = current_life_cycle / (float) ripe_time;
            fill = ripe;
        }else if (current_life_cycle > ripe_time && current_life_cycle <= old_time){
            int total_steps = old_time - ripe_time;
            int current_step = current_life_cycle - ripe_time;

            fill.r = ripe.r + (current_step * ((old.r - ripe.r) / total_steps));
            fill.g = ripe.g + (current_step * ((old.g - ripe.g) / total_steps));
            fill.b = ripe.b + (current_step * ((old.b - ripe.b) / total_steps));
        }else {
            fill = old;
            density = ((float) total_life_cycle - current_life_cycle) / (total_life_cycle - old_time);
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
    int maturity_time = 10;
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
    void step(GridCell **N, GridCell **E, GridCell **S, GridCell **W, int current_tick){
        if (density >= 1){
            if(!*N){
                *N = new LeafCell(get_pos_in_direction(north), window, 3, &season_allowable_new_growth, current_tick);
            }
            if(!*E){
                *E = new LeafCell(get_pos_in_direction(east), window, 3, &season_allowable_new_growth, current_tick);
            }
            if(!*S){
                *S = new LeafCell(get_pos_in_direction(south), window, 3, &season_allowable_new_growth, current_tick);
            }
            if(!*W){
                *W = new LeafCell(get_pos_in_direction(west), window, 3, &season_allowable_new_growth, current_tick);
            }
        }
        if (density < 1){
            density = ((float) current_tick - tick_born) / maturity_time;
        }else{
            density = 1;
        }

        current_season = (current_tick - tick_born) / maturity_time;

        if (current_season != past_season){
            season_allowable_new_growth = 1;
            past_season = current_season;
        }
    }
};
