#ifndef DESCENT_UTILITIES_MACROS_H
#define DESCENT_UTILITIES_MACROS_H

/**
 * @brief Converts its argument into a string literal
 * 
 * @param x The argument to be stringified.
 * @return A string literal representing the argument passed to this macro.
 * 
 * @note This macro will directly expand macros instead of the values that they represent
 */
#define STRINGIFY_PURE(x) #x

/**
 * @brief Expands and converts the value of its argument into a string literal
 * 
 * @param x The argument to be expanded and stringified
 * @return A string literal representing the expanded and stringified argument
 * 
 * @note This macro first expands the argument `x` (if it is a macro) and then converts
 * it into a string literal.
 */
#define STRINGIFY(x) STRINGIFY_PURE(x)

#endif
