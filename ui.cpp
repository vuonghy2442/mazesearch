#include "ui.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

void show_cursor(bool on) {
    if (on)
        std::cout << "\x1B[?25h";
    else
        std::cout << "\x1B[?25";
}

void gotoxy(int x, int y) {
    std::cout << "\x1B[" << x << ";" << y << "f";
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
    std::cout << "\x1B[48;5;" << c << "m";
}

void set_fg(int c) {
    std::cout << "\x1B[38;5;" << c << "m";
}

void reset_fg() {
   std::cout << "\x1B[39m";
}
void reset_bg() {
   std::cout << "\x1B[49m";
}

void reset_all() {
   std::cout << "\x1B[0m";
}