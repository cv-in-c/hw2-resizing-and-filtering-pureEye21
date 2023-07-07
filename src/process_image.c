#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    int idx = im.h * im.w * c + im.w * y + x;
    return im.data[idx];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // TODO Fill this in
    int idx = im.h * im.w * c + im.w * y + x;
    im.data[idx] = v;

}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    for(int i=0; i<im.c; i++){
        for(int j=0; j<im.h; j++){
            for(int k=0; k<im.w; k++){
                int idx = im.h * im.w * i + im.w * j + k;
                copy.data[idx] = im.data[idx];
            }
        }
    }

    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    for(int i=0; i<im.h; i++){
        for(int j=0; j<im.w; j++){
            int factor = im.h * im.w;
            int idx = im.w * i + j;
            gray.data[idx] = 0.299 * im.data[factor * 0 + idx] + 0.587 * im.data[factor * 1 + idx] + 0.144 * im.data[factor * 2 + idx];
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    for(int i=0; i<im.h; i++){
        for(int j=0; j<im.w; j++){
            int idx = im.h * im.w * c + im.w * i + j;
            im.data[idx] += v;
        }
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for(int i=0; i<im.c; i++){
        for(int j=0; j<im.h; j++){
            for(int k=0; k<im.w; k++){
                int idx = im.h * im.w * i + im.w * j + k;
                im.data[idx] = fmax(im.data[idx], 0);
                im.data[idx] = fmin(im.data[idx], 1);
            }
        }
    }

}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    for(int i=0; i<im.h; i++){
        for(int j=0; j<im.w; j++){
            // get rgb values for image
            float r = get_pixel(im, j, i, 0);
            float g = get_pixel(im, j, i, 1);
            float b = get_pixel(im, j, i, 2);
            
            // calculate Value
            float v = three_way_max(r, g, b);

            // calculate Saturation
            float m = three_way_min(r, g, b);
            float c = v - m;
            float s = (v == 0) ? 0 : c / v;

            // calculate Hue
            float h_dash = 0;
            if(c != 0){
                if(v == r){
                    h_dash = (g-b) / c;
                }
                else if(v == g){
                    h_dash = ((b-r) / c) + 2;
                }
                else if(v == b){
                    h_dash = ((r-g) / c) + 4;
                }
            }
            float h = h_dash / 6;
            if(h<0) h++;
            
            set_pixel(im, j, i, 0, h);
            set_pixel(im, j, i, 1, s);
            set_pixel(im, j, i, 2, v);
        }
    }

}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
     for (int i = 0; i < im.h; i++) {
        for (int j = 0; j < im.w; j++) {
            float h = get_pixel(im, j, i, 0);
            float s = get_pixel(im, j, i, 1);
            float v = get_pixel(im, j, i, 2);

            float c = s * v;
            float m = v - c;
            float h_dash = h * 6;
            float x = c * (1 - fabsf(fmodf(h_dash, 2) - 1));

            float r, g, b;
            if (h_dash < 1) {
                r = c;
                g = x;
                b = 0;
            } else if (h_dash < 2) {
                r = x;
                g = c;
                b = 0;
            } else if (h_dash < 3) {
                r = 0;
                g = c;
                b = x;
            } else if (h_dash < 4) {
                r = 0;
                g = x;
                b = c;
            } else if (h_dash < 5) {
                r = x;
                g = 0;
                b = c;
            } else {
                r = c;
                g = 0;
                b = x;
            }

            set_pixel(im, j, i, 0, r + m);
            set_pixel(im, j, i, 1, g + m);
            set_pixel(im, j, i, 2, b + m);
        }
    }
}
