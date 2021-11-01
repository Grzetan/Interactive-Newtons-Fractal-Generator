#include <iostream>
#include <fstream>
#include <stdlib.h>
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
    std::cout << "Compiling code..." << std::endl;

    //Create source file
    std::ofstream f ( "src/tmp.cpp" );
    f << "#include <iostream>\n#include <Complex.h>\n\nextern \"C\" Complex fx(Complex& z){\nComplex c = " + fx + ";\nreturn c; \n}\n\n" +
    "extern \"C\" Complex fprime(Complex& z){\nComplex c = " + fprime + ";\nreturn c; \n}";
    f.close();
    
    //Compile code
    system ("g++ -I./src src/tmp.cpp src/Complex.cpp -shared -fPIC -o src/tmp.so -ldl");
}

void generateFractal(){
    //Load created library      
    typedef Complex(*func_t)(Complex&);
    typedef Complex(*fprime_t)(Complex&);

    void * lib = dlopen ("./src/tmp.so", RTLD_LAZY);
    if (!lib) {
        std::cout << "Cannot open library: " << dlerror() << '\n';
        std::exit(0);
    }

    if (lib) {
        //Load functions
        func_t fun = (func_t) dlsym(lib, "fx");
        fprime_t fprime = (fprime_t) dlsym(lib, "fprime");
        //Start generation
        Complex z(-1,-1);
        Complex fx_v = fun(z);
        Complex fprime_v = fprime(z);
        Complex a = fx_v / fprime_v;
        a.print();
        dlclose ( lib );
    }
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

    //Compile
    compile(fx, fprime);

    //Load library
    generateFractal();

    return 0;
}