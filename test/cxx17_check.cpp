#include <any>
#include <variant>
#include <optional>

int main()
{
    std::any x;
    std::variant<int, float> y = 1;
    std::get<int>(y);
    std::optional<int> op;
    return 0;
}
