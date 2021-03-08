#version 400

uniform int window_width;
uniform int window_height;

uniform float x_translation;
uniform float y_translation;
uniform float zoom;
uniform int iterations = 255;


float map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

void main()
{

	float diversion_threshold = 4.0;

    float saved_brightness = 0.0;
    //float z_real = map(gl_FragCoord.x, 0.0, float(window_width), (-2.0 - x_translation) * zoom, (2.0 - x_translation) * zoom);
    //float z_imaginary = map(gl_FragCoord.y, 0.0, float(window_height), (-2.0 + y_translation) * zoom, (2.0 + y_translation) * zoom);

    float real_translated_a = ((-2.0 - x_translation));
    float real_translated_b = ((2.0 - x_translation));

    float imaginary_translated_a = ((-2.0 + y_translation));
    float imaginary_translated_b = ((2.0 + y_translation));

    float z_real = map(gl_FragCoord.x, 0.0, float(window_width), real_translated_a * zoom, real_translated_b * zoom);
    float z_imaginary = map(gl_FragCoord.y, 0.0, float(window_height), imaginary_translated_a * zoom, imaginary_translated_b * zoom);

    float z_real_original = z_real;
    float z_imaginary_original = z_imaginary;
    float z_real_temp;
    float z_imaginary_temp;

    float brightness = 0.0;

    bool draw = false;

    for (int n = 0;n <= iterations; n++) {
        z_real_temp = z_real * z_real - z_imaginary * z_imaginary;
        z_imaginary_temp = 2.0 * z_real * z_imaginary;

        z_real = z_real_temp + z_real_original;
        z_imaginary = z_imaginary_temp + z_imaginary_original;

        brightness = map(float(n), 0.0, float(iterations), 0.0, 1.0);
        brightness = map(sqrt(brightness), 0.0, 1.0, 1.0, 0.0);

        if (n > iterations/2) draw = true;

        if ((z_real * z_real + z_imaginary * z_imaginary) > diversion_threshold) {
            break;
        }
    }

    if(draw) gl_FragColor = vec4(int(brightness),  0.0, int(brightness), 1.0);
    else if (!draw) gl_FragColor = vec4(1.0,  1.0, 1.0, 1.0);
}