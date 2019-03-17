/*! \brief Templates for metaprogramming
 *
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 *
 *
 *
 *
 * Examples:
 *
 *      Determine the 2nd type in a type list.
 *
 *      nth_type_of<2, float, int, char, double>::type;        <--- char
 *
 *      first_type_of<float, int, char, double>::type          <--- float
 *
 *      last_type_of<float, int, char, double::type            <--- double
 *
 *      type_in<float, int, char double, float, bool>::value   <--- 1 (s
 *          -- determines if the first type is duplicated
 *             in the list. In this case, float is there twice.
 *
 */

#ifndef GNL_META_H
#define GNL_META_H

#include<type_traits>

//===============================================================================
// META-FUNCTIONS FOR EXTRACTING THE n-th TYPE OF A PARAMETER PACK

// Declare primary template
template<int I, typename... Ts>
struct nth_type_of
{};
// Base step
template<typename T, typename... Ts>
struct nth_type_of<0, T, Ts...>
{using type = T;};

// Induction step
template<int I, typename T, typename... Ts>
struct nth_type_of<I, T, Ts...>
{ using type = typename nth_type_of<I - 1, Ts...>::type;};

// Helper meta-function for retrieving the first type in a parameter pack
template<typename... Ts>
struct first_type_of
{using type = typename nth_type_of<0, Ts...>::type;};

// Helper meta-function for retrieving the last type in a parameter pack
template<typename... Ts>
struct last_type_of
{using type = typename nth_type_of<sizeof...(Ts) - 1, Ts...>::type;};


//========================================

// Meta functions for determining whether a type exists in a paramter pack
//
//  eg: type_in<float,    int, doulbe, char,float>::value  == true (since float exists in the pack)

template<typename T, typename ...Tail>
struct type_in
{};

template<typename T, typename Head>
struct type_in<T,Head>
{static const bool value = std::is_same<T,Head>::value;};

template<typename T, typename Head, typename ...Tail>
struct type_in<T,Head,Tail...>
{static const bool value = std::is_same<T,Head>::value || type_in<T, Tail...>::value;};

//===========================================
template<int I, typename... Ts>
struct pth_type_of
{};
// Base step
template<typename T, typename... Ts>
struct pth_type_of<0, T, Ts...>
{
    static const int value = std::is_same < T, typename first_type_of<Ts...>::type >::value ? 0 : -1;
};

// Induction step
template<int I, typename T, typename... Ts>
struct pth_type_of<I, T, Ts...>
{static const int value = std::is_same < T, typename nth_type_of<I, Ts...>::type >::value ? I : pth_type_of<I-1,T, Ts...>::value;};

// Meta function for determining the location (index) of a type within a paramter pack.
//                                 0     1       2
//  eg:  index_in_list<float,     int, float, double>::value == 1 since (float is type 1)
template<typename T, typename... Ts>
struct index_in_list
{static const int value = pth_type_of< sizeof...(Ts)-1, T, Ts...>::value;};

//================================

// meta function for determining the max and min size type in
// a parameter pack.
template<typename T, typename ...Tail>
struct type_size{};

template<typename T, typename Head>
struct type_size<T,Head>
{
    static const int max   = sizeof(T)>sizeof(Head) ? sizeof(T) : sizeof(Head);
    static const int min   = sizeof(T)<sizeof(Head) ? sizeof(T) : sizeof(Head);
    static const int sum   = sizeof(T) + sizeof(Head);
};

template<typename T, typename Head, typename ...Tail>
struct type_size<T,Head,Tail...>
{
    static const int max = sizeof(T) > type_size<Head,Tail...>::max ? sizeof(T) : type_size<Head,Tail...>::max;
    static const int min = sizeof(T) < type_size<Head,Tail...>::min ? sizeof(T) : type_size<Head,Tail...>::min;
    static const int sum = sizeof(T) + type_size<Head,Tail...>::sum;

};

#endif // META_H

