#include<stdio.h>
#include "src\image.h"
#include "src\stb_image_write.h"
#include "src\stb_image.h"
#include "src\args.h"

int main(){
    image im1 = load_image("dumbledore.png");
    image f = make_gaussian_filter(2);
    image lfreq1 = convolve_image(im1, f, 1);
    image hfreq1 = sub_image(im1, lfreq1);
    
    image im2 = load_image("ron.png");
    image lfreq2 = convolve_image(im2, f, 1);
    image hfreq2 = sub_image(im2, lfreq2);

    image reconstruct = add_image(lfreq2, hfreq1);
    // save_image(lfreq, "low-frequency");
    // save_image(hfreq, "high-frequency");
    save_image(reconstruct, "reconstruct2");
    return 0;
}