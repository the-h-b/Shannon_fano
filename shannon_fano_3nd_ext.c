#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_SYMBOLS 100
#define MAX_SIZE 100

typedef struct {
    char symbols[MAX_SYMBOLS * 2 + 1]; // Maximum length for the concatenated symbols
    float probability;
    char code[MAX_SIZE];
} CodeWord;

void shannonFano(CodeWord *words, int start, int end) {
    if (start >= end) return;

    float totalProb = 0;
    for (int i = start; i <= end; i++) {
        totalProb += words[i].probability;
    }

    float halfProb = 0;
    int split = start;
    for (int i = start; i <= end; i++) {
        halfProb += words[i].probability;
        if (halfProb >= totalProb / 2) {
            // Better balancing of the split
            if (fabs((totalProb / 2) - (halfProb - words[i].probability)) < fabs(halfProb - (totalProb / 2))) {
                split = i - 1;
            } else {
                split = i;
            }
            break;
        }
    }

    // Assign '0' to the first group and '1' to the second group
    for (int i = start; i <= end; i++) {
        strcat(words[i].code, (i <= split) ? "0" : "1");
    }

    // Recursively apply Shannon-Fano on each partition
    shannonFano(words, start, split);
    shannonFano(words, split + 1, end);
}

float calculateEntropy(CodeWord *words, int n) {
    float entropy = 0;
    for (int i = 0; i < n; i++) {
        if (words[i].probability > 0) {
           entropy -= words[i].probability * (float)log2(words[i].probability);
        }
    }
    return entropy;
}

void printCodes(CodeWord *words, int n) {
    printf("+---------+-------------+-------------+------+\n");
    printf("| Symbols | Probability | Code        | Length|\n");
    printf("+---------+-------------+-------------+------+\n");
    for (int i = 0; i < n; i++) {
        size_t codeLength = strlen(words[i].code);
        printf("| %-7s | %0.6f    | %-12s | %-4zu |\n", words[i].symbols, words[i].probability, words[i].code, codeLength);
    }
    printf("+---------+-------------+-------------+------+\n");
}

int main() {
    int n;
    printf("Enter the number of symbols: ");
    scanf("%d", &n);

    char symbols[MAX_SYMBOLS][2]; // Maximum length for a single symbol
    float probabilities[MAX_SYMBOLS];
    for (int i = 0; i < n; i++) {
        printf("Enter symbol name for s%d: ", i + 1);
        scanf("%s", symbols[i]);
    }

    float totalProbCheck = 0;
    for (int i = 0; i < n; i++) {
        printf("Enter the probability for %s: ", symbols[i]);
        scanf("%f", &probabilities[i]);
        totalProbCheck += probabilities[i];
    }

    if (fabs(totalProbCheck - 1.0) > 1e-6) {
        printf("Error: Total probabilities do not sum to 1.0\n");
        return 1;
    }

    int totalCombinations = n * n * n;
    CodeWord words[totalCombinations];
    int index = 0;

    // Generate all combinations and their probabilities
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                snprintf(words[index].symbols, sizeof(words[index].symbols), "%s%s%s", symbols[i], symbols[j], symbols[k]);
                words[index].probability = probabilities[i] * probabilities[j] * probabilities[k];
                words[index].code[0] = '\0'; // Initialize code string
                index++;
            }
        }
    }

    // Sort symbols by probability in descending order (bubble sort)
    for (int i = 0; i < totalCombinations - 1; i++) {
        for (int j = 0; j < totalCombinations - i - 1; j++) {
            if (words[j].probability < words[j + 1].probability) {
                CodeWord temp = words[j];
                words[j] = words[j + 1];
                words[j + 1] = temp;
            }
        }
    }

    shannonFano(words, 0, totalCombinations - 1);
    printCodes(words, totalCombinations);

    float entropy = calculateEntropy(words, totalCombinations);
    printf("Entropy of the system: %.4f bits/symbol\n", entropy);
    return 0;
}
