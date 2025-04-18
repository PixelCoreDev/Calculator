#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifndef _SSIZE_T_DEFINED
typedef long long ssize_t;
#define _SSIZE_T_DEFINED
#endif
// Custom implementation for getline for non-POSIX systems
ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || 
        stream == NULL) {
            
        return -1;
    }

    char *buffer = *lineptr;
    size_t size = *n;
    int c;
    size_t i = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (i + 1 >= size) {
            size = size ? size * 2 : 128;
            char *newBuffer = realloc(buffer, size);
            if (!newBuffer) {
                return -1;
            }
            buffer = newBuffer;
        }
        buffer[i++] = c;
        if (c == '\n') {
            break;
        }
    }

    if (i == 0 && c == EOF) {
        return -1;
    }

    buffer[i] = '\0';
    *lineptr = buffer;
    *n = size;
    return i;
}

// Function prototype
char* input(); 
char* errorCheck(char *array);
int calculate();

int main(){
    // Call the input function to read a term from the user
    char *array = input();


    printf("Array contents in main:\n");
    for (size_t i = 0; i < strlen(array); i++) {
        printf("%c ", array[i]);
    }

    errorCheck(array);

   
    
    
    free(array);
    return 0;
}
char* input(){
    char *input = NULL;
    size_t bufferSize = 0;

    printf("Enter a Term: ");
    // Use getline to dynamically allocate memory for input
    ssize_t inputLength = getline(&input, &bufferSize, stdin);
    if (inputLength == -1) {
        printf("Error reading input\n");
        free(input);
        return NULL;
    }

    // Remove the newline character if present
    if (input[inputLength - 1] == '\n') {
        input[inputLength - 1] = '\0';
        inputLength--;
    }

    // Check for empty input
    if (inputLength == 0) {
        printf("No input provided\n");
        free(input);
        return NULL;
    }

    // Dynamically allocate memory for the array based on input length
    char *array = (char *)malloc(sizeof(char) * (inputLength + 1)); // +1 for null-terminator
    if (array == NULL) {
        printf("Memory allocation failed\n");
        free(input);
        return NULL;
    }

    // Copy the input into the array safely
    #ifdef __STDC_LIB_EXT1__
        strncpy_s(array, inputLength + 1, input, inputLength);
    #else
        strncpy(array, input, inputLength);
        array[inputLength] = '\0'; // Ensure null-termination
    #endif

    // Print the array contents
    printf("Array contents:\n");
    for (size_t i = 0; i < inputLength; i++) {
        printf("%c ", array[i]);
    }
    printf("\n");

    // Free allocated memory
    free(input);
    
    
    return array;
}
char* errorCheck(char *array){
    

    int arithmetic = 0;
    int balance = 0;
    char inParanthesis = 0;

    for (size_t i = 0; i < strlen(array); i++) {
        switch (array[i]){
        case '(':
            balance++;
            inParanthesis = 1;
            break;
        case ')':
            balance--;
            break;
        
        case '*':   
        case '/':
        case '^':
        case ',':
            arithmetic++;
            break;
        default:
            inParanthesis = 0;
            arithmetic = 0;
        }
        if (arithmetic > 1) {
            printf("Error: Multiple arithmetic operators in a row\n");
            return NULL;
        }
        if (inParanthesis > 1) {
            printf("Error: Empty Paranthesis\n");
            return NULL;
        }

    }
    // Check for paretheses balance
    if (balance != 0) {
        printf("Error: Unmatched parentheses\n");
        return NULL;
    }

    
        return array;

    
}