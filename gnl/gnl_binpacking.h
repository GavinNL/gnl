#ifndef __GNL_BINPACKING_H__
#define __GNL_BINPACKING_H__

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


    Bin() : left(0), right(0)
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
        x = 0;
        y = 0;
        w = W;
        h = H;

    }

    Bin(T X, T Y, T W, T H) : left(0), right(0) , x(X), y(Y), w(W), h(H)
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
        //if(left)  delete left;
        //if(right) delete right;
    }

    void resize(T W, T H)
    {
        clear();
        x = 0;
        y = 0;
        w = W;
        h = H;

    }

    Rectangle insert(T W, T H, T padding)
    {
        Rectangle R;
        R.valid = false;

        if( W+2*padding > w || H+2*padding > h ) return R;

        if(left)
        {
            R = left->insert( W, H);
            if(R) return R;

            R = right->insert( W, H);
            if(R) return R;

        } else {

            if( W+padding <= w && H+padding<= h)
            {
                // it fits!
                left .reset( new Bin( x+W, y   , w-W,  H  , padding) );
                right.reset( new Bin( x  ,y+H ,  w  , h-H,  padding)  );
                // std::cout << "It fits! (" << W << "," << H << ") -->(" << w << "," << h << ")\n";
                R = {x,y,W,H,true};
                return( R );
            }
        }

        //R = {-1,-1,-1,-1};
        return( R );

    }



protected:
    std::unique_ptr<Bin> left;
    std::unique_ptr<Bin> right;

    float x;
    float y;
    float w;
    float h;

};

}



#endif // GNL_BINPACKING_H
