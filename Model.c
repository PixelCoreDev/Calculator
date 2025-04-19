#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

// Function declarations following our grammar:
double parseExpression(const char **s);  // handles '+' and '-'
double parseTerm(const char **s);        // handles '*' and '/' and implicit multiplication
double parseFactor(const char **s);      // handles '^' (right associative)
double parsePrimary(const char **s);     // handles numbers, parentheses, and unary +/-
char* calculate(const char *input);      // main calculation function


// int main(void) {
//     // Test input: "2^(-3)(-3)" should be interpreted as (2^(-3)) * (-3) = -0.375.
//     const char *input = "2^(-3)(-3)/0"; 
//     char *result = calculate(input);
//     printf("Result: %s\n", result);
//     printf("Test");
//     free(result); // Free the dynamically allocated result
//     return 0;
// }

// Expression ::= Term { ('+' | '-') Term }
double parseExpression(const char **s) {
    double value = parseTerm(s);
    while (**s == '+' || **s == '-') {
        char op = **s;
        (*s)++; // consume the operator
        double term = parseTerm(s);
        if (op == '+') {
            value += term;
        } else {
            value -= term;
        }
    }
    return value;
}

// Term ::= Factor { ('*' | '/') Factor | ImplicitMult Factor }
double parseTerm(const char **s) {
    double value = parseFactor(s);
    while (1) {
        // Handle explicit multiplication or division operators.
        if (**s == '*') {
            (*s)++; // consume '*'
            double factor = parseFactor(s);
            value *= factor;
        } else if (**s == '/') {
            (*s)++; // consume '/'
            double factor = parseFactor(s);
            if (factor == 0) {
                // Return an error message for division by zero
                printf("Error: Division by zero\n");
                exit(EXIT_FAILURE);
            }
            value /= factor;
        }
        // Implicit multiplication:
        // If the next non-space character appears to begin a primary expression,
        // then multiply implicitly.
        else if (**s == '(' || isdigit(**s) || **s == '-' || **s == '.') {
            double factor = parseFactor(s);
            value *= factor;
        }
        else {
            break;
        }
    }
    return value;
}

// Factor ::= Primary [ '^' Factor ]
// (Exponentiation is right associative.)
double parseFactor(const char **s) {
    double base = parsePrimary(s);
    if (**s == '^') {
        (*s)++; // consume '^'
        double exponent = parseFactor(s); // recursive call for right associativity
        base = pow(base, exponent);
    }
    return base;
}

// Primary ::= Number | '(' Expression ')' | ('+' | '-') Primary
double parsePrimary(const char **s) {
    // Skip spaces
    while (isspace(**s)) {
        (*s)++;
    }
    
    // Unary plus: simply skip it.
    if (**s == '+') {
        (*s)++;
        return parsePrimary(s);
    }
    
    // Unary minus: return negative of the primary.
    if (**s == '-') {
        (*s)++;
        return -parsePrimary(s);
    }
    
    // Parenthesized expression
    if (**s == '(') {
        (*s)++; // consume '('
        double value = parseExpression(s);
        if (**s == ')') {
            (*s)++; // consume ')'
        } else {
            printf("Error: missing closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
        return value;
    }
    
    // Otherwise, it should be a number.
    char *end;
    double value = strtod(*s, &end);
    if (*s == end) {
        // Nothing was parsed, so it's an error.
        printf("Error: expected a number but found '%c'\n", **s);
        exit(EXIT_FAILURE);
    }
    *s = end;
    return value;
}

// The main calculation function:
char* calculate(const char *input) {
    const char *s = input;
    char *result = (char *)malloc(256); // Allocate memory for the result string
    if (!result) {
        printf("Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Try to parse the expression
    double value = parseExpression(&s);
    snprintf(result, 256, "%f", value);
    return result;
}