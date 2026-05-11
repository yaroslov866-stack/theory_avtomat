#pragma once

#include "../symbol/sym.h"
#include "../vis/robot_vis.h"
#include <stack>
#include <iostream>
enum Action_type{
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    SEND_DRONES,
    GET_DRONES_COUNT,
    ASSIGN_VAR,
    CALL_FUNC,
    NOP
};
struct Action{
    Action_type type;
    std::string name_tar;
    int value;
    Action(): type(NOP),value(0){}
};

struct Map{
    std::vector<std::vector<int>> map;
    size_t map_width;
    size_t map_height;
    Map(int width,int height){
        map_height = height;
        map_width = width;
        map.resize(map_height,std::vector<int>(map_width,CELL_EMPTY));
        for(size_t i = 0;i<map_width;i++){
            map[0][i] = CELL_WALL;
            map[map_height-1][i] = CELL_WALL;
        }
        for(size_t i=0;i<map_height;i++){
            map[i][0] = CELL_WALL;
            map[i][map_width-1] = CELL_WALL;
        }
        map[3][3] = CELL_EXIT;
    }
};

struct RobotState{
    int x;
    int y;
    int drones_count;
    bool robot_broke;
    Map map;
    RobotState(int x,int y):x(x),y(y),drones_count(5),robot_broke(false),map(Map(10,10)){}
};

class Interpreter{
    public:
        RobotGraphic* gui;
        SymTable symbols;
        RobotState robot;
        std::unordered_map<std::string,std::vector<Action>> function;
        std::stack<std::string> call_stack;
        std::vector<std::string> targets;
    public:
        Interpreter(int startx,int starty):symbols(SymTable()),robot(RobotState(startx,starty)){
            gui = new RobotGraphic(robot.map.map_width, robot.map.map_height, &robot.map.map);
            gui->update(robot.x, robot.y);
        }
        void add_symbols(const std::string& name,Typee type,bool is_const,int init_value){
            symbols.add_symbol(name,type,is_const,init_value);
        }
        int get_int(const std::string& name){
            return symbols.get_int(name);
        }
        bool check_exit(){
            if(robot.map.map[robot.y][robot.x] == CELL_EXIT){
                std::cout<<"EXIT FOUND"<<std::endl;
                if(gui){
                    gui->draw_final();
                    sf::sleep(sf::milliseconds(700));
                    gui->close();
                } 
                return true;  
            }
            return false;
        }
        void move_left(int steps){
            if(robot.robot_broke){
                std::cerr<<"Robot_broke"<<std::endl;
                return;
            }
            std::cout<<"move left "<<steps<<std::endl;
            for(size_t i = 0;i<steps;i++){
                if(robot.map.map[robot.y][robot.x-1] == CELL_WALL){
                    std::cerr<<"Bag meet wall("<<robot.x-1<<","<<robot.y<<")"<<std::endl;
                    robot.robot_broke = true;
                    return;
                }
                robot.x -=1;
                gui->update(robot.x, robot.y);
                gui->handle_events();
                sf::sleep(sf::milliseconds(700));
            }
            if(check_exit()){
                robot.robot_broke = true;
                return;
            }
        }
        void move_right(int steps){
            if(robot.robot_broke){
                std::cerr<<"Robot_broke"<<std::endl;
                return;
            }
            std::cout<<"move right "<<steps<<std::endl;
            for(size_t i = 0;i<steps;i++){
                if(robot.map.map[robot.y][robot.x+1] == CELL_WALL){
                    std::cerr<<"Bag meet wall("<<robot.x+1<<","<<robot.y<<")"<<std::endl;
                    robot.robot_broke = true;
                    return;
                }
                robot.x +=1;
                gui->update(robot.x, robot.y);
                gui->handle_events();
                sf::sleep(sf::milliseconds(700));
            }
            if(check_exit()){
                robot.robot_broke = true;
                return;
            }
        }
        void move_up(int steps){
            if(robot.robot_broke){
                std::cerr<<"Robot_broke"<<std::endl;
                return;
            }
            std::cout<<"move up "<<steps<<std::endl;
            for(size_t i = 0;i<steps;i++){
                if(robot.map.map[robot.y-1][robot.x] == CELL_WALL){
                    std::cerr<<"Bag meet wall("<<robot.x<<","<<robot.y-1<<")"<<std::endl;
                    robot.robot_broke = true;
                    return;
                }
                robot.y -=1;
                gui->update(robot.x, robot.y);
                gui->handle_events();
                sf::sleep(sf::milliseconds(700));
            }
            if(check_exit()){
                robot.robot_broke = true;
                return;
            }
        }
        void move_down(int steps){
            if(robot.robot_broke){
                std::cerr<<"Robot_broke"<<std::endl;
                return;
            }
            std::cout<<"move down "<<steps<<std::endl;
            for(size_t i = 0;i<steps;i++){
                if(robot.map.map[robot.y+1][robot.x] == CELL_WALL){
                    std::cerr<<"Bag meet wall("<<robot.x<<","<<robot.y+1<<")"<<std::endl;
                    robot.robot_broke = true;
                    return;
                }
                robot.y +=1;
                gui->update(robot.x,robot.y);
                gui->handle_events();
                sf::sleep(sf::milliseconds(700));
            }
            if(check_exit()){
                robot.robot_broke = true;
                return;
            }
        }
        int get_drones_count(){
            return robot.drones_count;
        }
        void assign_variable(const std::string& name,int value){
            symbols.set_int(name,value);
        }
        void add_target(const std::string& name){
            targets.push_back(name);
        }
        void assign_to_targets(int value){
            for(auto& target:targets){
                assign_variable(target, value);
            }
            targets.clear();
        }
        void begin_function(const std::string& name){
            call_stack.push(name);
            function[name] = {};
        }
        
        void end_function(){
            if (!call_stack.empty()){
                call_stack.pop();
            }
        }
        
        void add_action(const Action& action){
            if(!call_stack.empty()){
                std::string current = call_stack.top();
                function[current].push_back(action);
            }
        }
        std::string current_function_name() {
            return call_stack.empty() ? "" : call_stack.top();
        }
        void execute_function(const std::string& name){
            if(robot.robot_broke){
                std::cerr << "Robot is broken" << std::endl;
                return;
            }
            if(function.find(name) == function.end()){
                std::cerr << "Function " << name << " not found" << std::endl;
                return;
            }
            call_stack.push(name);
            std::cout << "\nExecuting" << name << "!" << std::endl;
            
            for(auto& action : function[name]){
                execute_action(action);
                if (robot.robot_broke) break;
            }
            call_stack.pop();
            std::cout << "End fun: " << name << "\n" << std::endl;
        }
        void execute_action(const Action& action){
            switch(action.type) {
                case Action_type::MOVE_LEFT:move_left(action.value); break;
                case Action_type::MOVE_RIGHT:move_right(action.value); break;
                case Action_type::MOVE_UP:move_up(action.value); break;
                case Action_type::MOVE_DOWN:move_down(action.value); break;
                case Action_type::SEND_DRONES:send_drones(action.value); break;
                case Action_type::CALL_FUNC:execute_function(action.name_tar); break;
                case Action_type::ASSIGN_VAR:assign_variable(action.name_tar, action.value); break;
                default: break;
            }
        }
        void send_drones(int count){

        }

};
