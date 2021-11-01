#include <iostream>

class Complex{
public:
    double real, imag;
    Complex(double _real, double _imag);

    Complex(Complex*);

    Complex operator^(int power);

    Complex operator*(Complex b);

    Complex operator*(double b);

    Complex operator+(Complex b);

    Complex operator-(Complex b);

    Complex operator+(double b);

    Complex operator-(double b);

    Complex operator/(double b);

    Complex operator/(Complex b);

    void print();
};