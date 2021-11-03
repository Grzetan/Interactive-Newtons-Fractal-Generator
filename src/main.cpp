#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <math.h>
#include <vector>
#include <array>
#include "Complex.h"

using std::vector;
using std::array;

typedef Complex(*func_t)(Complex&);

struct f{
    func_t x;
    func_t prime;
};

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

f loadLibrary(){
    std::cout << "Loading library..." << std::endl;
    //Load created library      
    void * lib = dlopen ("./src/tmp.so", RTLD_LAZY);
    if (!lib) {
        std::cout << "Cannot open library: " << dlerror() << '\n';
        std::exit(0);
    }
    //Load functions
    func_t fx = (func_t) dlsym(lib, "fx");
    func_t fprime = (func_t) dlsym(lib, "fprime");
    return {fx, fprime};
}

void generateFractal(func_t fx, func_t fprime){
    std::cout << "Generating fractal..." << std::endl;

    //Constants
    const int WIDTH = 1000, HEIGHT = 1000, MAX_ITER = 100;
    double xmin = -1, xmax = 1, ymin = -1, ymax = 1;

    //Variables for pixel/coefficient transform
    double linspaceX[WIDTH], linspaceY[HEIGHT], stepX = (xmax - xmin) / WIDTH, stepY = (ymax - ymin) / HEIGHT, cofx, cofy;
    
    //Roots calculation
    double TOL = 1.e-8, dist;
    int closestRoot, i, j, k;
    Complex delta, _dist;
    vector<Complex> roots;
    bool foundRoot;

    //Roots colors
    vector<array<int,3>> rootsColors;

    //Output map
    int x = WIDTH * HEIGHT;
    int *pixels = new int[WIDTH * HEIGHT * 3];

    //Output file
    FILE *img;
    img = fopen("img.ppm", "wb");
    fprintf(img, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);

    srand(time(NULL));

    for(int x=0; x<WIDTH; x++){
        cofx = stepX * x;
        for(int y=0; y<HEIGHT; y++){
            foundRoot = false;
            cofy = stepY * y;
            Complex z(cofx, cofy);
            for(i=0; i<MAX_ITER; i++){
                delta = fx(z) / fprime(z);
                //If step is really small
                if(std::abs(delta.real) < TOL || std::abs(delta.imag) < TOL){
                    //Find closest root
                    closestRoot = -1;
                    for(j=0; j<roots.size(); j++){
                        _dist = z - roots[j];
                        dist = std::sqrt(_dist.real * _dist.real + _dist.imag * _dist.imag);
                        if(dist < 1){
                            closestRoot = j;
                            break;
                        }
                    }
                    if(closestRoot == -1){
                        //If there is no root, create new one
                        roots.push_back(z);
                        rootsColors.push_back({rand() % 255, rand() % 255, rand() % 255});
                        closestRoot = roots.size() - 1;
                    }
                    //Assing corresponding color to processed pixel
                    pixels[x*WIDTH + y] = rootsColors[closestRoot][0]; 
                    pixels[x*WIDTH + y + 1] = rootsColors[closestRoot][1]; 
                    pixels[x*WIDTH + y + 2] = rootsColors[closestRoot][2]; 
                    foundRoot = true;

                    break;
                }
                z = z - delta;
            }
            //If point isn't pulled by any root, color it black
            if(!foundRoot){
                pixels[x*WIDTH + y] = 0; 
                pixels[x*WIDTH + y + 1] = 0; 
                pixels[x*WIDTH + y + 2] = 0;   
            }
        }
    }
    std::cout << roots.size();
    free(pixels);
}

int main(int argc, char *argv[]){
    //Load equation
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

    //Compile generated code
    compile(fx, fprime);

    //Load library
    f F = loadLibrary();

    //Generate Fractal
    generateFractal(F.x, F.prime);

    return 0;
}