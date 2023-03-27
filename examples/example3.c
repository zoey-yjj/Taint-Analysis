int main() { // entry block
    int i, j=99, k=33, sink=55, source;
    // read source from input or initilized with a tainted value
    source = 1245678;
    i = 0;

    if (j > 1) {  // if.then block
        i = source * j / k - 111;
    } else {
        k = source % 333 + 123; // if.else block
        k *= source;
    }

    sink = i + k;  // if.end block
}
