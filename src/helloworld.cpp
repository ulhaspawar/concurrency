#include <iostream>
#include<thread>

using namespace std;

void test() {
    cout << "Hello World\n";
}

int main_t()
{
    thread t(test);

    t.join();

    cout << endl;
    return 0;
}