#include<mutex>
#include<iostream>

using namespace std;

int main() {
    mutex m;
    char c;
    cout<<(sizeof(m)* 5000);
    return 0;
}