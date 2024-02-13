#include <iostream>

void print_integers(int from, int to) {
    for(int i = from; i < to; i++)
        std::cout << i << " ";
}

int main(void) {
    std::cout << "hello, world\n";
    print_integers(0, 10);
    std::cout << "\n";
}
