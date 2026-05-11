#include <SFML/Graphics.hpp>
#include <vector>

class RobotGraphic{
    private:
        sf::RenderWindow window;
        int cell_size = 60;
        int map_width;
        int map_height;
        int robot_x;
        int robot_y;
        std::vector<std::vector<int>>* map;
        
    public:
        RobotGraphic(int w,int h,std::vector<std::vector<int>>* m):map_width(w),map_height(h),map(m),robot_x(1),robot_y(1){
            window.create(sf::VideoMode(w * 60, h * 60), "Robot GUI");
        }
        void update(int x, int y){
            robot_x = x;
            robot_y = y;
            draw();
        }
        
        void draw(){
            window.clear(sf::Color::White);
            for(int y = 0;y<map_height;y++){
                for(int x = 0;x<map_width;x++){
                    sf::RectangleShape cell(sf::Vector2f(cell_size-1,cell_size-1));
                    cell.setPosition(x*cell_size,y*cell_size);
                    switch((*map)[y][x]){
                        case 0: cell.setFillColor(sf::Color(175, 232, 160,150)); break; // EMPTY
                        case 1: cell.setFillColor(sf::Color(125, 96, 100)); break; // WALL
                        case 2: cell.setFillColor(sf::Color(216, 48, 242,150)); break;          // EXIT
                    }
                    window.draw(cell);
                }
            }
            sf::CircleShape robot(30);
            robot.setFillColor(sf::Color(86, 191, 252,150));
            robot.setPosition(robot_x*cell_size,robot_y*cell_size);
            window.draw(robot);
            
            window.display();
        }
        
        bool is_open(){return window.isOpen();}
        
        void handle_events(){
            sf::Event event;
            while(window.pollEvent(event)){
                if(event.type == sf::Event::Closed){
                    window.close();
                }
            }
        }
        void draw_final(){
            window.clear(sf::Color::White);
            for(int y = 0;y<map_height;y++){
                for(int x = 0;x<map_width;x++){
                    sf::RectangleShape cell(sf::Vector2f(cell_size-1,cell_size-1));
                    cell.setPosition(x*cell_size,y*cell_size);
                    switch((*map)[y][x]){
                        case 0:cell.setFillColor(sf::Color(175, 232, 160,150)); break; // EMPTY
                        case 1:cell.setFillColor(sf::Color(125, 96, 100)); break; // WALL
                        case 2:cell.setFillColor(sf::Color(216, 48, 242,150)); break;          // EXIT
                    }
                    window.draw(cell);
                }
            }
            sf::RectangleShape robot(sf::Vector2f(cell_size-1,cell_size-1));
            robot.setFillColor(sf::Color(194, 17, 4));
            robot.setPosition(robot_x*cell_size,robot_y*cell_size);
            window.draw(robot);
            window.display();
        }
        void close(){
            window.close();
        }
};