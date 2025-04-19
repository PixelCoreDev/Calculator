#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function prototype
char *input(const char *providedInput); 
char errorCheck(char *array);
//double calculate(char *array); not used now

int main(void) {
    enum bool{true = 1 , false = 0};

    const char *providedInput = "(2)*3"; // This is your pre-defined input
   
    char *array = input(providedInput);
    
    if (array == NULL) {
        return 1; // Exit if there is an error
    }
    if (errorCheck(array) == false) {
        free(array);
        return 1; // Exit if there is an error in the input
    }

    printf("Processed array contents in main:\n");
    fputs(array, stdout);

    free(array); // Free the allocated memory
    return 0;
}

char* input(const char *providedInput) {
    if (providedInput == NULL) {
        printf("Error: Input is NULL\n");
        return NULL;
    }

    size_t inputLength = strlen(providedInput);

    // Check for empty input
    if (inputLength == 0) {
        printf("Error: No input provided\n");
        return NULL;
    }

    // Dynamically allocate memory for the array based on input length
    char *array = (char *)malloc(sizeof(char) * (inputLength + 1)); // +1 for null-terminator
    if (array == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Copy the provided input into the array safely
    strncpy(array, providedInput, inputLength);
    array[inputLength] = '\0'; // Ensure null-termination

    return array; // Return the dynamically allocated array
}
char errorCheck(char *array){
    
    enum bool{true = 1 , false = 0};

    int arithmeticRepeating = false; // = 0
    int balance = true; // = 1
    char inParanthesis = true; // = 1

    size_t length = strlen(array);
    for (size_t i = 0; i < length; i++) {
        switch (array[i]){
        case '(':
            balance++; // Increment the balance when we encounter an opening parenthesis
            inParanthesis = false; // Set to false when we encounter an opening parenthesis to indicate that we are inside parentheses
            break;
        case ')':
            balance--; // Decrement the balance when we encounter a closing parenthesis

            if (balance < 1){
                printf("Error: closing paranthesis before opening\n");
                return 0; 
            }
            if (inParanthesis == false) {
                printf("Error: Empty Paranthesis\n");
                return 0; 
            }
            
            break;
        
        case '*':   
        case '/':
        case '^':
        case ',':
            // Check for repeated arithmetic operators
            if (arithmeticRepeating != false) {
                printf("Error: Multiple arithmetic operators in a row\n");   
            return 0;
            }
        arithmeticRepeating++; // Increment the arithmeticRepeating flag when we encounter an arithmetic operator
            break;
        default:
            inParanthesis = true; // Set to true when we encounter a non-operator character to indicate that something is inside parentheses
            arithmeticRepeating = false; // Reset the arithmeticRepeating flag when we encounter a non-operator character
        }

        
        
    }
    
    // Check for paretheses balance
    if (balance != true) {
        printf("Error: Unmatched parentheses\n");
        return 0;
    }

        return 1; 
}