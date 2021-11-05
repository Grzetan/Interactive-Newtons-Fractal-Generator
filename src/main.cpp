#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <math.h>
#include <vector>
#include <array>
#include <string.h>
#include "Complex.h"

using std::vector;
using std::array;

typedef Complex(*func_t)(Complex&);

struct f{
    func_t x;
    func_t prime;
};

struct Args{
    int width = 500;
    int height = 500;
    double zoom = 1;
    const char* ppm = "Newtons-Fractal.ppm";
    const char* png = "Newtons-Fractal.png";
    const char* equation;
};

void slice_str(const char * str, char * buffer, size_t start, size_t end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

Args parseArgs(int count, char* values[]){
    if(count < 2){
        std::cout << "You must provide polynomial\n";
        std::exit(0);
    }
    Args args;
    args.equation = values[1];
    for(int i=0; i<count; i++){
        if(strcmp(values[i], "--size") == 0){
            char* size = values[i+1];
            char w[strlen(size)+1];
            char h[strlen(size)+1];
            for(int j=0; j<strlen(size); j++){
                if(size[j] == 'x'){//
                    slice_str(size, w, 0, j-1);        
                    slice_str(size, h, j+1, strlen(size));  
                    args.width = std::stoi(w);
                    args.height = std::stoi(h);   
                    break;   
                }
            }
        }else if(strcmp(values[i], "--name") == 0){
            char* name = values[i+1];
            char* ppm;
            char* png;
            asprintf(&ppm, "%s%s", name, ".ppm");
            asprintf(&png, "%s%s", name, ".png");
            args.ppm = ppm;
            args.png = png;
        }else if(strcmp(values[i], "--zoom") == 0){
            args.zoom = 1 / std::stod(values[i+1]);
        }
    }
    return args;
}

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
    //Remove spaces
    for(int i=0; i<equation.length(); i++){
        if(equation[i] == ' '){
            equation.erase(equation.begin() + i);
        }
    }
    
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

void generateFractal(func_t fx, func_t fprime, Args args){
    //Constants
    const int WIDTH = args.width, HEIGHT = args.height, MAX_ITER = 1000;
    const double canvasX = 3 * args.zoom, canvasY = (float(HEIGHT) / float(WIDTH)) * 3 * args.zoom;
    const double xmin = -0.5*canvasX, xmax = 0.5*canvasX, ymin = -0.5*canvasY, ymax = 0.5*canvasY; 


    //Variables for pixel/coefficient transform
    double linspaceX[WIDTH], linspaceY[HEIGHT], stepX = canvasX / WIDTH, stepY = canvasY / HEIGHT, cofx, cofy;
    
    //Roots calculation
    double TOL = 1.e-12, rootTOL = 1.e-7,dist;
    int closestRoot, i, j, k, count = 0;
    Complex delta, _dist;
    vector<Complex> roots;
    bool foundRoot;

    //Roots colors
    vector<array<int,3>> rootsColors;

    //Output pixel
    static unsigned char pixel[3];

    //Output file
    FILE *img;
    img = fopen(args.ppm, "wb");
    fprintf(img, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);

    srand(time(NULL));

    time_t start = time(0);

    for(int y=0; y<HEIGHT; y++){
        cofy = ymin + stepY * y;
        for(int x=0; x<WIDTH; x++){
            count++;
            std::cout << "\rGenerating fractal... " << int((float(float(count) / float(WIDTH*HEIGHT))) * 100) << "%";
            foundRoot = false;
            cofx = xmin + stepX * x;
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
                        if(dist < rootTOL){
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
                    pixel[0] = rootsColors[closestRoot][0];
                    pixel[1] = rootsColors[closestRoot][1]; 
                    pixel[2] = rootsColors[closestRoot][2]; 
                    foundRoot = true;

                    break;
                }
                z = z - delta;
            }
            //If point isn't pulled by any root, color it black
            if(!foundRoot){
                pixel[0] = 0; 
                pixel[1] = 0; 
                pixel[2] = 0;   
            }
            fwrite(pixel, 1, 3, img);
        }
    }
    std::cout << "\nGenerated in: " << float(time(0) - start) << "s" << std::endl;
    fclose(img);
    char* shell;
    asprintf(&shell, "convert %s %s", args.ppm, args.png);
    system(shell);
    asprintf(&shell, "rm %s", args.ppm);
    system(shell);
}

int main(int argc, char *argv[]){
    //Parse args
    Args args = parseArgs(argc, argv);
    std::string fx = "", fprime = "";

    //Generate f(x) and it's derivative
    processEquation(argv[1], fx, fprime);

    //Compile generated code
    compile(fx, fprime);

    //Load library
    f F = loadLibrary();

    //Generate Fractal
    generateFractal(F.x, F.prime, args);

    return 0;
}