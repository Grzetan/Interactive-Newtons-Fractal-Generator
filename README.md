# Interactive Newtons Fractal Generator

Visaulization of Newton's Fractal written in C/C++. This program generates Newton's fractal based on polynomial provided in parameters and saves it to `.png` image.

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

For better expirience i recommend you to watch awesome [3Blue1Brown's video](https://www.youtube.com/watch?v=-RdOwhmqP5s&t=709s) about Newton's fractal
and read [Wikipedia article](https://en.wikipedia.org/wiki/Newton_fractal).

# How to use 

Download `generate` file from root directory and call it
```bash
./generate "z^3 - 1"
```

If for some reason it doesn't work you can complie source code localy:

```bash
g++ -I/src src/main.cpp src/Complex.cpp -o generate -ldl
```
and call program as shown above.

# Parameters

## -Polymonial

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

## -Size (Not required)

Specify desired size of output image.

Default value is 500px x 500px

```bash
--size widthxheight
```

Where `width` and `height` are `int`

Example:

```bash
./generate "z^3 - 1" --size 1000x1000
```
This will generate image of size 1000px x 1000px

## -Name (Not required)

Specify name of output image

Default value is `Newtons-Fractal`

```bash
--name name
```
Where `name` is string

Example:

```bash
./generate "z^3 - 1" --name my_name_for_fractal
```
This will save the result in `my_name_for_fractal.png`

## -Zoom (Not required)

Specify zoom of image

Default value is `1`

```bash
--zoom zoom
```
where `zoom` is `double`

If `zoom < 1` image will be zoomed out.

If `zoom > 1` image will be zoomed in.

Example:
```bash
./generate "z^3 - 1" --zoom 0.1
```
This will zoom out 10 times.

Example:
```bash
./generate "z^3 - 1" --zoom 10
```
This will zoom in 10 times.

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

