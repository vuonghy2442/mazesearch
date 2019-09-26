#include "ui.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

void show_cursor(bool on) {
    if (on)
        std::wcout << L"\e[?25h";
    else
        std::wcout << L"\e[?25l";
}

void gotoxy(int x, int y) {
    std::wcout << L"\e[" << x << L";" << y << L"f";
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
    std::wcout << L"\e[48;5;" << c << L"m";
}

void set_fg(int c) {
    std::wcout << L"\e[38;5;" << c << L"m";
}

void reset_fg() {
   std::wcout << L"\e[39m";
}
void reset_bg() {
   std::wcout << L"\e[49m";
}

void reset_all() {
   std::wcout << L"\e[0m";
}