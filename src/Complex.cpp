#include "Complex.h"

Complex::Complex(double _real, double _imag){
    real = _real;
    imag = _imag;
}

Complex::Complex(){
    
}

Complex::Complex(Complex*){

}

Complex Complex::operator^(int power){
    Complex base(real, imag);
    for(int i=0; i<power-1; i++){
        base = operator*(base);
    }
    return base;
}

Complex Complex::operator*(Complex b){
    return Complex(real * b.real - imag * b.imag, real * b.imag + imag * b.real);
};

Complex Complex::operator*(double b){
    return Complex(real * b, imag * b);
}

Complex Complex::operator+(Complex b){
    return Complex(real + b.real, imag + b.imag);
}

Complex Complex::operator-(Complex b){
    return Complex(real - b.real, imag - b.imag);
}

Complex Complex::operator+(double b){
    return Complex(real + b, imag);
}

Complex Complex::operator-(double b){
    return Complex(real - b, imag);
}

Complex Complex::operator/(double b){
    return Complex(real / b, imag / b);
}

Complex Complex::operator/(Complex b){
    double denominator = (b.real * b.real + b.imag * b.imag);
    return Complex((real * b.real + imag * b.imag) / denominator,
    (imag * b.real - real * b.imag) / denominator);
}

Complex Complex::sinh(){
    return Complex(std::sinh(real) * std::cos(imag), std::cosh(real) * std::sin(imag));
}

Complex Complex::cosh(){
    return Complex(std::cosh(real) * std::cos(imag), std::sinh(real) * std::sin(imag));
}

void Complex::print(){
    std::cout << real << " + " << imag << "i" << std::endl;
}
