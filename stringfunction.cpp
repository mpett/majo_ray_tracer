#include <stdio.h>
#include <string>

using namespace std;

string fname;

void myStringFunction(fname) {
    printf("fname");
}


int main() {
    myStringFunction("mamma");

    return 0;
}
