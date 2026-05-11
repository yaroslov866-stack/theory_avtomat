#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
enum Typee{
    TYPE_INT = 0,
    TYPE_BOOL = 1,
    TYPE_CELL = 2
};

enum CellType{
    CELL_EMPTY = 0,
    CELL_WALL = 1,
    CELL_EXIT = 2,
    CELL_UNDEF = 3
};

struct Symbol{
    public:
        std::string name;
        Typee type;
        bool is_const;
        int int_value;
        bool bool_value;
        CellType cell_value;
        bool is_array;
        std::vector<int> dimensions;
        std::vector<int> array_data; // 0-int,1-bool,2-cell
        Symbol():is_const(false),int_value(0),bool_value(false),cell_value(CELL_EMPTY),is_array(false){}
};


class SymTable{
    private:
        std::unordered_map<std::string,Symbol> table;
    public:
        void add_symbol(const std::string& name,Typee type,bool is_const,int init_value){
            Symbol sym;
            sym.name = name;
            sym.type = type;
            sym.is_const =is_const;
            sym.is_array = false;
            switch(type){
            case TYPE_INT:
                sym.int_value = init_value;
                break;
            case TYPE_CELL:
                sym.cell_value = static_cast<CellType>(init_value);
                break;
            case TYPE_BOOL:
                sym.bool_value = init_value;
                break;
            default:
                break;
            }
            table[name] = sym;
        }
        bool findSym(const std::string& name){
            return table.find(name)!=table.end();
        }
        void set_int(const std::string& name,int value){
            if(table.find(name) == table.end()){
                std::cerr << "Variable  " << name << "  not found" << std::endl;
                return;
            }
            Symbol& sym = table[name];
            if(sym.is_const){
                std::cerr << "Сonstant  " << name<< std::endl;
                return;
            }
            switch(sym.type){
                case TYPE_INT:
                    sym.int_value = value;
                    break;
                case TYPE_BOOL:
                    sym.bool_value = (value != 0);
                    break;
                case TYPE_CELL:
                    sym.cell_value = static_cast<CellType>(value);
                    break;
            }
            std::cout << "  " << name << "=" << value << std::endl;
        }
        int get_int(const std::string& name){
            if(table.find(name) == table.end()){
                std::cerr << "Error: variable  " << name << "  not found" << std::endl;
                return 0;
            }
            Symbol& sym = table[name];
            switch(sym.type) {
                case TYPE_INT: return sym.int_value;
                case TYPE_BOOL: return sym.bool_value ? 1 : 0;
                case TYPE_CELL: return static_cast<int>(sym.cell_value);
            }
            return 0;
        }
};