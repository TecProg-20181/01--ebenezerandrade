#include <stdio.h>

typedef struct _pixel {
    unsigned short int red;
    unsigned short int green;
    unsigned short int blue;
} Pixel;

typedef struct _image {
    unsigned short int pixel[512][512][3];
    unsigned int w;
    unsigned int h;
} Image;

Image grey_scale(Image img) {
 
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            int average = img.pixel[i][j][0] +
                        img.pixel[i][j][1] +
                        img.pixel[i][j][2];
            average /= 3;
            img.pixel[i][j][0] = average;
            img.pixel[i][j][1] = average;
            img.pixel[i][j][2] = average;
        }
    }

    return img;
}

void apply_blur_to_image(unsigned int height,
                        unsigned short int pixel[512][512][3],
                        int T,
                        unsigned int width) {

    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            Pixel average = {0, 0, 0};

            int lower_height = (height - 1 > i + T/2) ? i + T/2 : height - 1;
            int minimum_width = (width - 1 > j + T/2) ? j + T/2 : width - 1;
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= lower_height; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= minimum_width; ++y) {
                    average.red += pixel[x][y][0];
                    average.green += pixel[x][y][1];
                    average.blue += pixel[x][y][2];
                }
            }

            average.red /= T * T;
            average.green /= T * T;
            average.blue /= T * T;

            pixel[i][j][0] = average.red;
            pixel[i][j][1] = average.green;
            pixel[i][j][2] = average.blue;
        }
    }
}

Image rotate90right(Image img) {
    Image rotated;

    rotated.w = img.h;
    rotated.h = img.w;

    for (unsigned int i = 0, y = 0; i < rotated.h; ++i, ++y) {
        for (int j = rotated.w - 1, x = 0; j >= 0; --j, ++x) {
            rotated.pixel[i][j][0] = img.pixel[x][y][0];
            rotated.pixel[i][j][1] = img.pixel[x][y][1];
            rotated.pixel[i][j][2] = img.pixel[x][y][2];
        }
    }

    return rotated;
}

void invert_colors(unsigned short int pixel[512][512][3],
                    unsigned int w, unsigned int h) {
    for (unsigned int i = 0; i < h; ++i) {
        for (unsigned int j = 0; j < w; ++j) {
            pixel[i][j][0] = 255 - pixel[i][j][0];
            pixel[i][j][1] = 255 - pixel[i][j][1];
            pixel[i][j][2] = 255 - pixel[i][j][2];
        }
    }
}

Image crop_image(Image img, int x, int y, int w, int h) {
    Image cut;

    cut.w = w;
    cut.h = h;

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            cut.pixel[i][j][0] = img.pixel[i + y][j + x][0];
            cut.pixel[i][j][1] = img.pixel[i + y][j + x][1];
            cut.pixel[i][j][2] = img.pixel[i + y][j + x][2];
        }
    }

    return cut;
}


int main() {
    Image img;

    // read type of image
    char p3[4];
    scanf("%s", p3);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &img.w, &img.h, &max_color);

    // read all pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            scanf("%hu %hu %hu", &img.pixel[i][j][0],
                                 &img.pixel[i][j][1],
                                 &img.pixel[i][j][2]);

        }
    }

    int number_of_options;
    scanf("%d", &number_of_options);

    for(int i = 0; i < number_of_options; ++i) {
        int option_value;
        scanf("%d", &option_value);

        switch(option_value) {
            case 1: { // grey_scale
                img = grey_scale(img);
                break;
            }
            case 2: { // sepia filter
                for (unsigned int x = 0; x < img.h; ++x) {
                    for (unsigned int j = 0; j < img.w; ++j) {
                        unsigned short int pixel[3];
                        pixel[0] = img.pixel[x][j][0];
                        pixel[1] = img.pixel[x][j][1];
                        pixel[2] = img.pixel[x][j][2];

                        int p =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
                        int smaller_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][0] = smaller_r;

                        p =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
                        smaller_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][1] = smaller_r;

                        p =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
                        smaller_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][2] = smaller_r;
                    }
                }

                break;
            }
            case 3: { // apply blur in image
                int size_of_image = 0;
                scanf("%d", &size_of_image);
                apply_blur_to_image(img.h, img.pixel, size_of_image, img.w);
                break;
            }
            case 4: { // Rotate
                int number_of_rotations = 0;
                scanf("%d", &number_of_rotations);
                number_of_rotations %= 4;
                for (int j = 0; j < number_of_rotations; ++j) {
                    img = rotate90right(img);
                }
                break;
            }
            case 5: { // mirroring
                int horizontal = 0;
                scanf("%d", &horizontal);

                int w = img.w, h = img.h;

                if (horizontal == 1) w /= 2;
                else h /= 2;

                for (int i2 = 0; i2 < h; ++i2) {
                    for (int j = 0; j < w; ++j) {
                        int x = i2, y = j;

                        if (horizontal == 1) y = img.w - 1 - j;
                        else x = img.h - 1 - i2;

                        Pixel aux1;
                        aux1.red = img.pixel[i2][j][0];
                        aux1.green = img.pixel[i2][j][1];
                        aux1.blue = img.pixel[i2][j][2];

                        img.pixel[i2][j][0] = img.pixel[x][y][0];
                        img.pixel[i2][j][1] = img.pixel[x][y][1];
                        img.pixel[i2][j][2] = img.pixel[x][y][2];

                        img.pixel[x][y][0] = aux1.red;
                        img.pixel[x][y][1] = aux1.green;
                        img.pixel[x][y][2] = aux1.blue;
                    }
                }
                break;
            }
            case 6: { // invert colors 
                invert_colors(img.pixel, img.w, img.h);
                break;
            }
            case 7: { // Cut Imagem
                int x, y;
                scanf("%d %d", &x, &y);
                int w, h;
                scanf("%d %d", &w, &h);

                img = crop_image(img, x, y, w, h);
                break;
            }
        }

    }

    // print type of image
    printf("P3\n");
    // print width height and color of image
    printf("%u %u\n255\n", img.w, img.h);

    // print pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            printf("%hu %hu %hu ", img.pixel[i][j][0],
                                   img.pixel[i][j][1],
                                   img.pixel[i][j][2]);

        }
        printf("\n");
    }
    return 0;
}
