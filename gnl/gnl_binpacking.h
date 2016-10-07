/*
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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
