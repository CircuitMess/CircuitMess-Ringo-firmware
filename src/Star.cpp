#include "Star.h"
/*
The MIT License (MIT)
Copyright (c) 2012, 2013 Brendon J. Brewer, Dan Foreman-Mackey,
and David W. Hogg.
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/**
 * Randomizes the position and speed of the star.
 */
void Star::randomize(int xMin, int xMax, int yMin, int yMax, int speedMin, int speedMax)
{
    // Generate random values for the star.
    x = random(xMin, xMax);
    y = random(yMin, yMax);
    speed = random(speedMin, speedMax+1); // speedMax is +1 to make it inclusive.
}

/**
 * Updates the position of the star.
 */
void Star::update()
{
    // Update the star's Y position based on its speed.
    y += speed;
}