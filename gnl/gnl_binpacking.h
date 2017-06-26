/*
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
 */

#ifndef GNL_BINPACKING_H
#define GNL_BINPACKING_H

#include <memory>


namespace gnl
{

template<typename T>
class Bin
{

public:
    struct Rectangle
    {
        T x;
        T y;
        T w;
        T h;

        bool valid;

        operator bool() const
        {
            return valid;
        }

    } ;


    Bin()
    {
    }

    float getUsedArea()
    {
        float area = 0.0;

        if(left)  area += left->getUsedArea();
        if(right) area += right->getUsedArea();

        area += w*h;
        return area;
    }

    Bin(T W, T H)
    {
        x = static_cast<T>(0);
        y = static_cast<T>(0);
        w = W;
        h = H;

    }

    Bin(T X, T Y, T W, T H) : x(X), y(Y), w(W), h(H)
    { }

    ~Bin()
    {
        // std::cout << "Destroying Bin\n";
        clear();
    }

    void clear()
    {
        left.reset();
        right.reset();
    }

    void resize(T W, T H)
    {
        clear();
        x = static_cast<T>(0);
        y = static_cast<T>(0);
        w = W;
        h = H;

    }

    Rectangle insert(T W, T H, T padding=static_cast<T>(0))
    {
        Rectangle R;
        R.valid = false;

        W += 2*padding;
        H += 2*padding;
        // failed
        if( W > w || H > h ) return R;

        if(left)
        {
            R = left->insert( W, H);
            if(R) return R;

            R = right->insert( W, H);
            if(R) return R;

        } else {

            if( W <= w && H<= h)
            {
                // it fits!
                 left.reset( new Bin( x+W, y   ,  w-W,  H )  );
                right.reset( new Bin( x  , y+H ,  w  ,  h-H)  );

                R.x = x+padding;
                R.y = y+padding;
                R.w = W-2*padding;
                R.h = H-2*padding;
                R.valid = true;
                return( R );
            }
        }

        //R = {-1,-1,-1,-1};
        return( R );

    }



protected:
    std::unique_ptr<Bin> left;
    std::unique_ptr<Bin> right;

    T x;
    T y;
    T w;
    T h;

};

}



#endif // GNL_BINPACKING_H
