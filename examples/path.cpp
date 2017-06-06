#include <iostream>
#include <gnl/gnl_path.h>
#include <assert.h>
#include <thread>


#include <cstdlib>

using namespace std;


int x[] ={1,2,3,4,5};

int main()
{

    int * y = x;

    y = y+2;
    std::cout << *y << std::endl;

    std::cout << gnl::Path::Home() << std::endl;
    std::cout << gnl::Path::Temp() << std::endl;






    return 0;


}

