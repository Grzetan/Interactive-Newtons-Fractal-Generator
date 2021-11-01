#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "Complex.h"

void preprocessChunk(std::string chunk, std::string& fx, std::string& fprime){
    std::string multi, pow, sign;

    if(chunk.find('z') != std::string::npos){
        multi = chunk.substr(1, chunk.find('z')-1);
        if(multi == ""){
            multi = "1";
        }
        sign = chunk[0];
        if(chunk.find('^') != std::string::npos){
            pow = chunk.substr(chunk.find('^')+1, chunk.length() - 1);
            fx = fx + sign + "(z^" + pow + ")*" + multi;
            fprime = fprime + sign + "(z^" + std::to_string(stoi(pow) - 1) + ")*" + std::to_string(stoi(multi) * stoi(pow));
        }else{
            fx = fx + sign + "z*" + multi;
            fprime = fprime + sign + multi;
        }
    }else{
        fx = fx + chunk;
    }
}

void processEquation(std::string equation, std::string& fx, std::string& fprime){
    int lastChunk = 0;
    if(equation[0] != '-'){
        equation = "+" + equation;
    }
    for(int i=0; i<equation.length(); i++){
        if(equation[i] == '+' || equation[i] == '-'){
            preprocessChunk(equation.substr(lastChunk, i - lastChunk), fx, fprime);
            lastChunk = i;
        }
    }
    preprocessChunk(equation.substr(lastChunk, equation.length()), fx, fprime);

    //Prepare beggining of the equation
    if(fx[0] == '+'){
        fx.erase(0,1);
    }else if(fx[0] == '-'){
        fx.erase(0,1);
        fx.insert(fx.find("*")+1, "-");
    }

    if(fprime[0] == '+'){
        fprime.erase(0,1);
    }else if(fprime[0] == '-'){
        fprime.erase(0,1);
        fprime.insert(fprime.find("*")+1, "-");
    }
}

void compile(std::string fx, std::string fprime){
    std::cout << "Compiling code..." << std::endl;;
    std::ofstream f ( "src/tmp.cpp" );
    f << "#include <iostream>\n#include <Complex.h>\n\nComplex fx(Complex& z){\nComplex c = " + fx + ";\nreturn c; \n}\n\n" +
    "Complex fprime(Complex& z){\nComplex c = " + fprime + ";\nreturn c; \n}";
    f.close();
    system ("/usr/bin/g++ -I./src -shared -fPIC src/tmp.cpp src/Complex.cpp -o src/tmp.so");
    // // load library       
    // void * lib = dlopen ("./libtmp.so", RTLD_NOW);
    // if (!lib) {
    //     std::cout << "Cannot open library: " << dlerror() << '\n';
    //     std::exit(0);
    // }

    // if (lib) {
    //     void* fun = dlsym(lib, "fun");

    //     // if (fun) {
    //     //     fun(3);
    //     // }
    //     dlclose ( lib );
    // }
}

int main(int argc, char *argv[]){
    std::string equation = argv[1];
    std::string fx = "", fprime = "";

    //Remove spaces
    for(int i=0; i<equation.length(); i++){
        if(equation[i] == ' '){
            equation.erase(equation.begin() + i);
        }
    }

    //Generate f(x) and it's derivative
    processEquation(equation, fx, fprime);
    // std::cout << fx << std::endl;
    // std::cout << fprime << std::endl;
    //Compile
    compile(fx, fprime);

    return 0;
}