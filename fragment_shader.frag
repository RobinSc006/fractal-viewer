#version 400

uniform int window_width;
uniform int window_height;

uniform float x_translation;
uniform float y_translation;
uniform float zoom;
uniform int iterations = 255;
uniform int color;


float map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

void main()
{

    double actZoom =  double(zoom * 0.000001);
    actZoom = actZoom * actZoom;

    double actX_trans =  double(x_translation * 0.00001);
    double actY_trans =  double(y_translation * 0.00001);

	float diversion_threshold = 4.0;

    double real_translated_a = (-2.0 - actX_trans);
    double real_translated_b = (2.0 - actX_trans);

    double imaginary_translated_a = (-2.0 + actY_trans);
    double imaginary_translated_b = (2.0 + actY_trans);

    double z_real = actX_trans + (gl_FragCoord.x/window_width - 0.5) * actZoom;
    double z_imaginary = actY_trans + (gl_FragCoord.y/window_height - 0.5) * actZoom;

    double z_real_original = z_real;
    double z_imaginary_original = z_imaginary;
    double z_real_temp;
    double z_imaginary_temp;

    float brightness = 0.0;

    for (int n = 0;n <= iterations; n++) {
        z_real_temp = z_real * z_real - z_imaginary * z_imaginary;
        z_imaginary_temp = 2.0 * z_real * z_imaginary;

        z_real = z_real_temp + z_real_original;
        z_imaginary = z_imaginary_temp + z_imaginary_original;

        if (color != 0)brightness = map(float(n), 0.0, float(iterations), 0.0, 1.0);
        else brightness = map(float(n), 0.0, float(iterations), 1.0, 0.0);

        brightness = map(sqrt(brightness), 0.0, 1.0, 0.0, 1.0);

        if ((z_real * z_real + z_imaginary * z_imaginary) > diversion_threshold) {
            break;
        }
    }

    if (color == 0) gl_FragColor = vec4(brightness, brightness, brightness, 1.0);
    if (color == 1) gl_FragColor = vec4(brightness, 0.0, 0.0, 1.0);
    if (color == 2) gl_FragColor = vec4(0.0, 0.0, brightness, 1.0);
    if (color == 3) gl_FragColor = vec4(0.0, brightness, 0.0, 1.0);
    if (color == 4) gl_FragColor = vec4(brightness, 0.0, brightness * 0.67, 1.0);
    if (color == 5) gl_FragColor = vec4(brightness * 0.1, brightness, brightness * 0.56, 1.0);
    if (color == 6) {
        if (brightness <= 0.2) {
            gl_FragColor = vec4(brightness * 0.10, 0.0, brightness * 0.10, 1.0);
        }
        else if (brightness <= 0.3) {
            gl_FragColor = vec4(brightness * 0.55, 0.0, brightness * 0.33, 1.0);
        }
        else if (brightness <= 0.5) {
            gl_FragColor = vec4(brightness * 0.23, brightness * 0.5, brightness * 0.67, 1.0);
        }
        else if (brightness <= 0.7) {
            gl_FragColor = vec4(brightness * 0.25, brightness * 0.25, brightness * 0.50, 1.0);
        }
        else if (brightness <= 0.8) {
            gl_FragColor = vec4(brightness * 0.33, brightness, brightness * 0.36, 1.0);
        }
        else {
            gl_FragColor = vec4(brightness * 0.20, 0.0, brightness * 0.05, 1.0);
        }
    }
}