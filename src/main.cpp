#include <iostream>

#include "Console.h"
#include "Context_Container.h"
#include "Window.h"

//Create new context
Context context;

void init() {
    context.init();
}

void loop() {

    context.loop();
    Console::Draw();
}

int main()
{
    //Responsible for setting up openGL context, window, and running loop
    Window window("Scientific Field Visualizer", init, loop);

    return 0;
}