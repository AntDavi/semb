#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 1024
#define TOLERANCE 0.01


// Algoritmo de distorção
float distortion(float x, float level) {
    float y = x * level;
    if (y > 1.0) {
        y = 1.0;
    } else if (y < -1.0) {
        y = -1.0;
    }
    return y;
}

int compare(float *input, float *output, int size, float tolerance) {
    int errors = 0;
    int i;
    for (i = 0; i < size; i++) {
        float diff = fabs(input[i] - output[i]);
        if (diff > tolerance) {
            printf("Erro na posição %d: entrada=%f, saída=%f\n", i, input[i], output[i]);
            errors++;
        }
    }
    return errors;
}

void saveOutputToFile(float *output, int size, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }
    for (int i = 0; i < size; i++) {
        fprintf(file, "%f\n", output[i]);
    }
    fclose(file);
}

void plotGraph() {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        fprintf(gnuplotPipe, "set title 'Onda de Entrada e Saída'\n");
        fprintf(gnuplotPipe, "set xlabel 'Amostra'\n");
        fprintf(gnuplotPipe, "set ylabel 'Amplitude'\n");
        fprintf(gnuplotPipe, "plot 'input.txt' with lines title 'Entrada', 'output.txt' with lines title 'Saída'\n");
        fflush(gnuplotPipe);
        fprintf(gnuplotPipe, "exit\n");
        pclose(gnuplotPipe);
    } else {
        printf("Erro ao abrir a tubulação do Gnuplot.\n");
    }
}

int main() {
    float input[SIZE];
    float output[SIZE];
    float level = 0.5;
    int i;

    // Preencher o vetor de entrada com valores de uma simulação de um sinal com frequência fixa
    float freq = 440.0; // frequência em Hz
    float phase = 0.0; // fase inicial em radianos
    float delta = (2 * 3.14159265358979323846 * freq) / 20000; // incremento de fase em cada amostra
    for (i = 0; i < SIZE; i++) {
        input[i] = sin(phase);
        phase += delta;
    }

    // Aplicar a distorção ao sinal de entrada e armazenar no vetor de saída
    for (i = 0; i < SIZE; i++) {
        output[i] = distortion(input[i], level);
    }

    // Verificar se o algoritmo está funcionando corretamente
    int errors = compare(input, output, SIZE, TOLERANCE);
    if (errors > 0) {
        printf("O algoritmo apresentou %d erro(s).\n", errors);
    } else {
        printf("O algoritmo está funcionando corretamente.\n");
    }

    // Salvar o vetor de entrada e saída em arquivos
    saveOutputToFile(input, SIZE, "input.txt");
    saveOutputToFile(output, SIZE, "output.txt");

    // Plotar o gráfico com o Gnuplot
    plotGraph();

    return 0;
}
