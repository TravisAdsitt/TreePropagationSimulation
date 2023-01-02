#include "config.hpp"
#include "utils.hpp"
#include "plants.hpp"

#ifndef WORLD_H
#define WORLD_H

class World{
    private:
    GridCell **grid;
    int grid_width, grid_height;
    sf::Vector2f block_area;
    sf::RenderWindow *window;
    SeedCell *new_trunk;
    public:
    World(sf::RenderWindow *window, int starting_seed_count = config::starting_seed_count) : grid_width(config::grid_edge_size), grid_height(config::grid_edge_size), window(window){
        // Initialize our grid
        grid = (GridCell **)malloc(sizeof(GridCell*) * (grid_width * grid_height));
        for (int i = 0; i < (grid_width * grid_height); i++) grid[i] = NULL;

        for(int t_i = 0; t_i < starting_seed_count;){
            int trunk_x = (int) (get_rand_float() * grid_width);
            int trunk_y = (int) (get_rand_float() * grid_height);
            if (!grid[(trunk_y * grid_width) + trunk_x]){
                grid[(trunk_y * grid_width) + trunk_x] = new SeedCell(sf::Vector2f(trunk_x, trunk_y), window, 0);
                t_i++;
            }
        }

    }
    void step(int current_tick){
        for (int y = 0; y < grid_height; y++){
            for (int x = 0; x < grid_width; x++){
                GridCell *curr_pointer = grid[(y * grid_width) + x];
                if(curr_pointer){
                    if(instanceof<SeedCell>(curr_pointer)){
                        delete curr_pointer;
                        grid[(y * grid_width) + x] = NULL;

                        if(get_rand_bool(0.01) || true){
                            grid[(y * grid_width) + x] = new TrunkCell(sf::Vector2f(x,y), window, current_tick);
                        }
                    }else{
                        sf::Vector2f curr_n = curr_pointer->get_pos_in_direction(north);
                        sf::Vector2f curr_s = curr_pointer->get_pos_in_direction(south);
                        sf::Vector2f curr_e = curr_pointer->get_pos_in_direction(east);
                        sf::Vector2f curr_w = curr_pointer->get_pos_in_direction(west);

                        int n_index = (curr_n.y * grid_width) + curr_n.x;
                        int s_index = (curr_s.y * grid_width) + curr_s.x;
                        int e_index = (curr_e.y * grid_width) + curr_e.x;
                        int w_index = (curr_w.y * grid_width) + curr_w.x;

                        curr_pointer->step(&grid[n_index], &grid[e_index], &grid[s_index], &grid[w_index], current_tick);
                    }
                    
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

#endif