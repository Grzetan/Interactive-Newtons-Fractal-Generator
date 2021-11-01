#include <iostream>
#include "Complex.h"

int main(int argc, char *argv[]){
    Complex x(-0.9,3.6);
    Complex z = (x^3) * 16 + 2000;
    z.print();
    return 0;
}