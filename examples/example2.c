int main() {

    int i=0, j=1, k=2, sink=123, source=1234567;  // entry block
    i = source;

    if (j > 1) {    // if.then block
        // skip;
    } else {        // if.else block
        k = i;
    }

    sink = k;       // if.end block
}