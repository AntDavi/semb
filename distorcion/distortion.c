#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024

void generate_plot(float* input, float* output) {
    FILE *gnuplot = popen("gnuplot -persist", "w");
    if (gnuplot == NULL) {
        printf("Erro ao iniciar o gnuplot.\n");
        return;
    }

    // Gerando gráfico de comparação
    fprintf(gnuplot, "set multiplot layout 2, 1\n");
    fprintf(gnuplot, "set title 'Forma de Onda de Entrada'\n");
    fprintf(gnuplot, "plot '-' with lines\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        fprintf(gnuplot, "%f\n", input[i]);
    }
    fprintf(gnuplot, "e\n");

    fprintf(gnuplot, "set title 'Forma de Onda de Saída'\n");
    fprintf(gnuplot, "plot '-' with lines\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        fprintf(gnuplot, "%f\n", output[i]);
    }
    fprintf(gnuplot, "e\n");

    fflush(gnuplot);
    pclose(gnuplot);
}

void generate_table(float* input, float* output) {
    printf("\nTabela de Comparação:\n");
    printf("---------------------\n");
    printf("  Amostra  |  Entrada  |   Saída   |\n");
    printf("----------------------------------\n");
    for (int i = 0; i < BUFFER_SIZE; i += 50) {
        printf("   %4d    | %8.4f  | %8.4f |\n", i, input[i], output[i]);
    }
}

void apply_distortion(float * input, float * output) {
    float frequency = 440.0;
    float amplitude = 0.8;
    float distortion_factor = 0.9;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        float time = (float)i / SAMPLE_RATE;
        float value = sin(2.0 * 3.1415 * frequency * time);

        // Aplicando distorção
        if (value >= distortion_factor)
            output[i] = distortion_factor;
        else if (value <= -distortion_factor)
            output[i] = -distortion_factor;
        else
            output[i] = value;
        
        // Amplificando o sinal
        output[i] *= amplitude;
    }
}

int main() {
    float input[BUFFER_SIZE];
    float output[BUFFER_SIZE];

    // Gerando vetor de entrada
    for (int i = 0; i < BUFFER_SIZE; i++) {
        float time = (float)i / SAMPLE_RATE;
        input[i] = sin(2.0 * 3.1415 * 440.0 * time);
    }

    // Aplicando distorção
    apply_distortion(input, output);

    // Gerando gráfico e tabela
    generate_plot(input, output);
    generate_table(input, output);

    return 0;
}