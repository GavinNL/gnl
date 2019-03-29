#include <any>
#include <variant>

int main()
{
    std::any x;
    std::variant<int, float> y = 1;
    std::get<int>(y);
    return 0;
}
