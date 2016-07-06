#ifndef GNL_BINPACKING_H
#define GNL_BINPACKING_H

#include <memory>

namespace gnl
{


class Bin
{


public:
    typedef struct
    {
        float x;
        float y;
        float w;
        float h;
    } Rectangle;


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

    Bin(float W, float H, float pad=0) : left(0), right(0)
    {
        x = 0;
        y = 0;
        w = W;
        h = H;
        padding = pad;
    }

    Bin(float X, float Y, float W, float H, float pad=0) : left(0), right(0)
    {
        x = X;
        y = Y;
        w = W;
        h = H;
        padding = pad;
    }

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

    void resize(float W, float H, float pad=0)
    {
        clear();
        x = 0;
        y = 0;
        w = W;
        h = H;
        padding = pad;
    }

    Rectangle insert(float W, float H)
    {
        Rectangle R = {-1,-1,-1,-1};

        if( W > w || H > h ) return R;

        if(left)
        {
            R = left->insert( W, H);
            if(R.w > 0) return R;

            R = right->insert( W, H);
            if(R.w > 0) return R;

        } else {

            if( W+padding <= w && H+padding<= h)
            {
                // it fits!
                left  = std::make_shared<Bin>( x+W, y   , w-W,  H  , padding);
                right = std::make_shared<Bin>( x  ,y+H ,  w  , h-H, padding);
                // std::cout << "It fits! (" << W << "," << H << ") -->(" << w << "," << h << ")\n";
                R = {x,y,W,H};
                return( R );
            }
        }

        R = {-1,-1,-1,-1};
        return( R );

    }



protected:
    std::shared_ptr<Bin> left;
    std::shared_ptr<Bin> right;

    float padding;
    float x;
    float y;
    float w;
    float h;

};

};



#endif // GNL_BINPACKING_H
