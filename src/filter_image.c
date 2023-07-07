#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                sum += get_pixel(im, x, y, c);
            }
        }
    }

    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                set_pixel(im, x, y, c, get_pixel(im, x, y, c) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    image filter = make_image(w, w, 1);

    for (int y = 0; y < filter.h; y++) {
        for (int x = 0; x < filter.w; x++) {
            set_pixel(filter, x, y, 0, 1.0 / (w * w));
        }
    }

    return filter;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c);

    int padding = filter.w / 2;
    int width = im.w + 2 * padding;
    int height = im.h + 2 * padding;
    image padded = make_image(width, height, im.c);

    if (preserve) {
        for (int c = 0; c < im.c; c++) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (x < padding || x >= width - padding || y < padding || y >= height - padding) {
                        set_pixel(padded, x, y, c, 0);
                    } else {
                        set_pixel(padded, x, y, c, get_pixel(im, x - padding, y - padding, c));
                    }
                }
            }
        }
    } else {
        for (int c = 0; c < im.c; c++) {
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    set_pixel(padded, x, y, c, get_pixel(im, x - padding, y - padding, c));
                }
            }
        }
    }

    image convolved = make_image(im.w, im.h, im.c);

    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float sum = 0;
                for (int j = -padding; j <= padding; j++) {
                    for (int i = -padding; i <= padding; i++) {
                        float im_value = get_pixel(padded, x + padding + i, y + padding + j, c);
                        float filter_value = get_pixel(filter, padding + i, padding + j, (filter.c == 1) ? 0 : c);
                        sum += im_value * filter_value;
                    }
                }
                set_pixel(convolved, x, y, c, sum);
            }
        }
    }

    free_image(padded);

    return convolved;
}

image make_highpass_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 4);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_sharpen_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_emboss_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 2, 1, 0, 1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 2, 2, 0, 2);
    return filter;
}

image make_gaussian_filter(float sigma)
{
    int size = (int)ceilf(sigma * 6);
    if (size % 2 == 0) size++;
    image filter = make_image(size, size, 1);

    float sum = 0;
    for (int y = 0; y < filter.h; y++) {
        for (int x = 0; x < filter.w; x++) {
            float u = x - (size - 1) / 2;
            float v = y - (size - 1) / 2;
            float value = expf(-(u * u + v * v) / (2 * sigma * sigma));
            set_pixel(filter, x, y, 0, value);
            sum += value;
        }
    }

    l1_normalize(filter);

    return filter;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result = make_image(a.w, a.h, a.c);

    for (int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                float val_a = get_pixel(a, x, y, c);
                float val_b = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, val_a + val_b);
            }
        }
    }

    return result;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);

    image result = make_image(a.w, a.h, a.c);

    for (int c = 0; c < a.c; c++) {
        for (int y = 0; y < a.h; y++) {
            for (int x = 0; x < a.w; x++) {
                float val_a = get_pixel(a, x, y, c);
                float val_b = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, val_a - val_b);
            }
        }
    }

    return result;
}

image make_gx_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 2);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

image make_gy_filter()
{
    image filter = make_image(3, 3, 1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

void feature_normalize(image im)
{
    float min_val = INFINITY;
    float max_val = -INFINITY;

    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float val = get_pixel(im, x, y, c);
                if (val < min_val) min_val = val;
                if (val > max_val) max_val = val;
            }
        }
    }

    float range = max_val - min_val;

    if (range == 0) return;

    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float val = get_pixel(im, x, y, c);
                set_pixel(im, x, y, c, (val - min_val) / range);
            }
        }
    }
}

image *sobel_image(image im)
{
    image *sobel = calloc(2, sizeof(image));

    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    sobel[0] = convolve_image(im, gx_filter, 0);
    sobel[1] = convolve_image(im, gy_filter, 0);

    free_image(gx_filter);
    free_image(gy_filter);

    return sobel;
}

image colorize_sobel(image im)
{
    image *sobel = sobel_image(im);

    image mag = make_image(im.w, im.h, 1);
    image theta = make_image(im.w, im.h, 1);

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float gx = get_pixel(sobel[0], x, y, 0);
            float gy = get_pixel(sobel[1], x, y, 0);
            float magnitude = sqrtf(gx * gx + gy * gy);
            float angle = atan2f(gy, gx);
            set_pixel(mag, x, y, 0, magnitude);
            set_pixel(theta, x, y, 0, angle);
        }
    }

    feature_normalize(mag);

    image colorized = make_image(im.w, im.h, 3);

    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float magnitude = get_pixel(mag, x, y, 0);
            float angle = get_pixel(theta, x, y, 0);
            set_pixel(colorized, x, y, 0, angle / TWOPI);
            set_pixel(colorized, x, y, 1, magnitude);
            set_pixel(colorized, x, y, 2, magnitude);
        }
    }

    hsv_to_rgb(colorized);

    free_image(*sobel);
    free_image(*(sobel + 1));
    free(sobel);

    free_image(mag);
    free_image(theta);

    return colorized;
}