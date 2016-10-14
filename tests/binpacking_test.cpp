#include <gnl/gnl_binpacking.h>
#include <iostream>

int main(int argc, char ** argv)
{

    gnl::Bin<int> B(100,100);

    {
        auto r = B.insert( 10, 10, 2);
        if(r)
        {
            std::cout << r.x << " "
                      << r.y << " "
                      << r.w << " "
                      << r.h << std::endl;
        }
    }

    {
        auto r = B.insert( 10, 10, 2);
        if(r)
        {
            std::cout << r.x << " "
                      << r.y << " "
                      << r.w << " "
                      << r.h << std::endl;
        }
    }

}
