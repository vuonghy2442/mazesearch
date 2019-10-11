#include "ui.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

void show_cursor(bool on) {
    if (on)
        std::cout << "\e[?25h";
    else
        std::cout << "\e[?25";
}

void gotoxy(int x, int y) {
    std::cout << "\e[" << x << ";" << y << "f";
}

void clear_screen(){
    if(system("clear")) {
        throw std::runtime_error("Cannot clear screen");
    }
}

void wait(int milisec){
    std::this_thread::sleep_for(std::chrono::milliseconds(milisec));
}

void set_bg(int c) {
    std::cout << "\e[48;5;" << c << "m";
}

void set_fg(int c) {
    std::cout << "\e[38;5;" << c << "m";
}

void reset_fg() {
   std::cout << "\e[39m";
}
void reset_bg() {
   std::cout << "\e[49m";
}

void reset_all() {
   std::cout << "\e[0m";
}