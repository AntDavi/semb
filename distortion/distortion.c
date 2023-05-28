#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_SAMPLES 1024
#define AMPLITUDE 0.5
#define SAMPLE_RATE 44100
#define NUM_HARMONICS 5
#define FREQUENCY 440.0
#define PI 3.1415926536

void geraOnda(float *input)
{
    /** Função que gera valores digitais para representar uma senóide
     *
     * Itera a quantidade de amostras definida para gerar uma senóide
     * na frequência também definida
     * Input:
     * - Vetor [float *input] que, manipulado, representará o formato de onda captado
     * pela entrada
     */
    float timeStep = 1.0 / SAMPLE_RATE; // Aplica a defasagem adequada à taxa de amostragem definida

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        float t = i * timeStep;
        input[i] = sin(2 * 3.1415 * FREQUENCY * t);
    }
}

float fatorial(int n)
{
    /** Função que calcula o valor do fatorial do número [n] recebido
     *
     * Input:
     * - Número [int n] que deverá ser obtido o valor do Fatorial
     * Output:
     * - Resultado [float fat] do calculo
     */
    int fat;
    for (fat = 1; n > 1; n = n - 1)
        fat = fat * n;
    return fat * 1.0;
}

float calcularSeno(float angulo)
{ /**
    * Função que calcula o seno do [angulo] a partir do método de aproximação de series de Taylor
    *
    * Input:
    * - Angulo [float angulo] em graus que deverá ser obtido o valor do Seno
    * Output:
    * - Resultado [float resultado] do calculo de aproximação,
    * com erro no máximo na 4ª casa decimal
    */
    float radiano = (angulo / 180.0) * PI; // Graus para radianos
    float resultado = 0.0;
    float termo;
    float potencia;
    int i, j, voltas, mult = 1;

    // Utilizando-se da propriedade ciclica do seno para obter um valor baixo
    // entre 0 e 2PI (Evitando erro no calculo da potenciação com números muito altos)
    voltas = radiano / (2 * PI);
    radiano -= 2 * PI * voltas;

    // Utilizando-se da propriedade de congruencia do circulo trigonométrico
    // para realizar calculos somente entre 0 e PI/2 (1º quadrante - Q1)
    // Para evitar calculos com numeros muito altos
    if (radiano > PI / 2 && radiano < PI)
    {
        radiano -= 2 * radiano - PI;
    }
    else if (radiano > PI && radiano < 3 * PI / 2)
    {
        radiano -= PI;
        mult = -1; // garantindo os valores negativos do seno para valores no Q3
    }
    else if (radiano > 3 * PI / 2)
    {
        radiano = 2 * PI - radiano;
        mult = -1; // garantindo os valores negativos do seno para valores no Q4
    }

    // Aplicando a aproximação por series de Taylor
    // sin(x) = SOMATORIO_0_i((-1^i) * x^(2i+1)/)
    //                       (  fatorial(2i+1)  )
    for (i = 0; i < 4; i++)
    {
        // Utilizando apenas 4 iterações da série de taylor
        // pois começaria a chegar a valores muito altos, comprometendo a precisão
        potencia = 1;
        for (j = 0; j < 2 * i + 1; j++)
        { // Calculo da potencia no numerador de cada termo
            potencia *= radiano;
        }
        termo = potencia / fatorial(2 * i + 1);

        if (i % 2 == 0)
        { // para termos de indice par, adiciona o termo ao resultado
            resultado += termo;
        }
        else
        { // para termos de indice impar, subtrai o termo do resultado
            resultado -= termo;
        }
    }

    return resultado * mult; // Como o calculo está ajustado para o Q1 multiplica-se pelo sinal do quadrante correto
}

void applyHarmonicDistortion(float *input, float *output)
{
    /**
     * Função que será utilizada no Microcontrolador para aplicar distorção
     *
     * Aplica os cálculos de transformação de onda com distorção harmônica
     * Input:
     * - Vetor [float *input] que representa o formato de onda captado
     * pela entrada
     * - Vetor [float *output] que será manipulado para o novo formato de onda
     * Output:
     * - Vetor [float *output] que representa o novo formato de onda (distorção)
     */
    float timeStep = 1.0 / SAMPLE_RATE;
    float harmonic;
    float valorSeno;
    int i, j;
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        float t = i * timeStep;
        float distortedSample = 0.0;

        for (j = 1; j <= NUM_HARMONICS; j++)
        {
            harmonic = j * FREQUENCY;
            // Como o valor de [t] deixa os numeros muito baixos
            // Aplicamos uma "defasagem" de 360° pois mantêm a congruencia do angulo

            valorSeno = calcularSeno(360 * harmonic * t);
            distortedSample += valorSeno / j;
        }

        output[i] = input[i] + distortedSample * AMPLITUDE;
    }
}

void applyHarmonicDistortionsSIMPLIFICADO(float *input, float *output)
{
    /**
     * FUNÇÃO UTILIZANDO A BIBLIOTECA MATH
     * Utilização: Obter os valores exatos que deverão ser obtidos na saída
     * Aplica os calculos precisos para os valores da trasnsformação do formato de onda
     * Input:
     * - Vetor [float *input] que representa o formato de onda captado
     * pela entrada
     * Output:
     * - Vetor [float *output] que representa o novo formato de onda (distorção)
     */
    float timeStep = 1.0 / SAMPLE_RATE;
    float harmonic;
    float valorSeno;
    int i, j;

    for (i = 0; i < NUM_SAMPLES; i++)
    {
        float t = i * timeStep;
        float distortedSample = 0.0;

        for (j = 1; j <= NUM_HARMONICS; j++)
        {
            harmonic = j * FREQUENCY;
            // É aplicada uma "defasagem" de 2PI para deixar o valor em radianos, e mantendo a congruencia do angulo
            valorSeno = sin(2 * PI * harmonic * t);
            distortedSample += valorSeno / j;
        }

        output[i] = input[i] + distortedSample * AMPLITUDE;
    }
}

int verificacao(float *input, float *outputObtida, int qtdItens, float limiteErro)
{
    int i, totalErros = 0;
    float diff;
    float outputCorreta[qtdItens];

    applyHarmonicDistortionsSIMPLIFICADO(input, outputCorreta);

    printf("-------------------------------------------\n");
    printf("  VERIFICANDO CORRETUDE DOS DADOS OBTIDOS\n");
    printf("-------------------------------------------\n");
    printf("   REFERENCIA  |   VALOR    |   DIFERENCA\n");
    printf("-------------------------------------------\n");

    for (i = 0; i < qtdItens; i++)
    {
        diff = outputCorreta[i] - outputObtida[i];

        if (diff > limiteErro)
        {
            totalErros++;
            printf(
                " %10.6f    | %9.6f  |%10.6f\n",
                outputCorreta[i],
                outputObtida[i],
                diff);
        }
    }

    printf("-------------------------------------------\n");
    printf("  AMOSTRAS COM ERROS %20d\n", totalErros);
    return totalErros;
}

void printTable(float *input, float *output)
{
    printf("Input\t\tOutput\n");

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        printf("%f\t%f\n", input[i], output[i]);
    }
}

void plotGraphSingle(float *samples, const char *title)
{
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    fprintf(gnuplotPipe, "set terminal png\n");
    fprintf(gnuplotPipe, "set output '%s.png'\n", title);
    fprintf(gnuplotPipe, "plot '-' with lines title '%s'\n", title);

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        fprintf(gnuplotPipe, "%d %f\n", i, samples[i]);
    }

    fprintf(gnuplotPipe, "e\n");

    fflush(gnuplotPipe);
    pclose(gnuplotPipe);
}

int main()
{
    float input[NUM_SAMPLES];  // Vetor que representa a os valores captados
    float output[NUM_SAMPLES]; // Vetor que representa o novo formato de onda

    geraOnda(input);

    applyHarmonicDistortionsSIMPLIFICADO(input, output);
    plotGraphSingle(output, "applyHarmonicDistortionsSIMPLIFICADO");

    
    applyHarmonicDistortion(input, output);
    plotGraphSingle(output, "applyHarmonicDistortion");

    verificacao(input, output, NUM_SAMPLES, 0.0001);

    printTable(input, output);
    plotGraphSingle(input, "input");

    return 0;
}