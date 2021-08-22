#include <stdio.h>
#include <string.h>

char fname;

void myStringFunction(fname) {
    printf(fname);
}

int main() {
    myStringFunction("mamma");

    return 0;
}