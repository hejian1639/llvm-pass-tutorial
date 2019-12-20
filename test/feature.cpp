#include <iostream>
#include <string>

class MyClass {
public:
    std::string s = "Hello, world\n"; // Non-static data member initializer
};

int main() {
    std::cout << MyClass().s;
}
