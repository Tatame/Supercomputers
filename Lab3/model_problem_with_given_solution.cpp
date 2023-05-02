#include <iostream>



int main(int argc, char **argv){

    int Nx = (argc > 1 ? std::stoi(argv[1]) : 500);
    int Ny = (argc > 2 ? std::stoi(argv[2]) : 500);

    int N = Nx * Ny;


    return 0;
}

