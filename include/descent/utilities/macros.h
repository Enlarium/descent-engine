/* Copyright 2025 XavierHarkonnen9 and Enlarium
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
