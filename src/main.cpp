#include <iostream>
#include <algorithm>
#include "Complex.h"

std::string fx = "0", fprime = "0", multi, pow, sign;

void preprocessChunk(std::string chunk){
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

void processEquation(std::string equation){
    int lastChunk = 0;
    if(equation[0] != '-'){
        equation = "+" + equation;
    }
    for(int i=0; i<equation.length(); i++){
        if(equation[i] == '+' || equation[i] == '-'){
            preprocessChunk(equation.substr(lastChunk, i - lastChunk));
            lastChunk = i;
        }
    }
    preprocessChunk(equation.substr(lastChunk, equation.length()));
}

void compile(std::string equation){
    
}

int main(int argc, char *argv[]){
    std::string equation = argv[1];

    //Remove spaces
    equation.erase(std::remove(equation.begin(), equation.end(), ' '), equation.end());

    //Preprocess equation
    processEquation(equation);
    std::cout << fx << std::endl;
    std::cout << fprime << std::endl;
    //Compile
    compile(equation);

    return 0;
}