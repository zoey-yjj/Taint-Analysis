int main() {
    int i, j, sink, source, N;   // entry block
    int a = 111;
    source = 1234567;  // read source from input
    N = 321;

    while (a < N) {             // while.cond block
                                // while.body block
        if (a % 2 == 1) {       // if.then block
            i = source * a / 2 + 777;

        } else {                // if.else block
            j = i + 3;
        }

        a++;                    // if.end block
    }

    sink = j;                   // while.end block
}
