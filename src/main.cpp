#include <iostream>
#include "Window.h"

void loop() {

}

int main()
{
    //Responsible for setting up openGL context, window, and running loop
    Window window("Scientific Field Visualizer", loop);

    return 0;
}