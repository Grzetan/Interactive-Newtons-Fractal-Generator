# Interactive Newtons Fractal Generator

Visaulization of Newton's Fractal written in C/C++. This program generates Newton's fractal based on polynomial provided in parameters and saves it in file 
called `Newtons-Fractal-123456789.png` where this long number is current timestamp.

# Requirements

- Linux (tested on Ubuntu 20.04 LTS)

- g++ compiler
```bash
sudo apt intall g++
```

- ImageMagick
```bash
sudo apt install imagemagick
```

- Basic polynomials understanding :D
```bash
sudo learn polynomials
```

# How to use 

Download `generate` file from root directory and call it
```bash
./generate "z^3 - 1"
```

If for some reason it doesn't work you can complie source code localy:

```bash
g++ -I/src src/main.cpp src/Complex.cpp -o generate -ldl
```
and call program following steps above.

# Parameters

### Polymonial

To generate fractal you have to provide this argument.
Just put it in quotes after `./generate` like this:
```bash
./generate "z^4 - 1"
```

#### Formula for each component:
```bash
az^n
```
where:
- a is an integer
- z is variable for polynomial (don't change it)
- n is a power of `z`

#### Examples:
```bash
./generate "-2z^14 - 2z^2 - z - 9"
```
```bash
./generate "5z^9 + 2z^8 - z^4 - 9"
```
```bash
./generate "-z^5 + 2z^4 - 19z^3 + z^2 - 18z - 2"
```

You can pick any polynomial as long as:
#### - it is in general form
This is **NOT** allowed:
```bash
./generate "(z - 9)(z + 2)(z - 12)(z + 16)"
```
#### - it's powers are whole, already calculated numbers (ints)
This is **NOT** allowed:
```bash
./generate "z^9*2 + 2z^2/8 - z^1/2 - 9"
```
#### - there aren't any trygonometric functions
This is **NOT** allowed:
```bash
./generate "sin(z^9) + 2cos(z^8) - tan(z^4) - 9"
```

# How it works

Program contains 4 steps:

### Polynomial preprocessing
It takes polynomial provided in parameters and generates code compatible with `Complex` library and saves it to `tmp.cpp`.

### Compiling
Then it compiles generated code and creates library called `tmp.so`

### Loading library
Next step is loading generated library so generator can use functions created by parsing provided polynomial.

### Generating fractal
Last but not least it generates fractal. If you are interested in how exacly this works I strongly recommend you
awesome [3Blue1Brown's video](https://www.youtube.com/watch?v=-RdOwhmqP5s&t=709s) about this topic. Every time different colors are picked,
so you can easly say that your fractal is unique😊. When fractal is generated
it is saved to `.ppm` file. For user's convinience it is later converted to `.png` file and `.ppm` file is deleted.

