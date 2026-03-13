#include <iostream>
#include "Window.h"
#include "Console.h"

void loop() {

    Console::Draw();
}

int main()
{
    //Responsible for setting up openGL context, window, and running loop
    Window window("Scientific Field Visualizer", loop);

    return 0;
}