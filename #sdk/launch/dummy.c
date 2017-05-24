__declspec(dllexport)
int main(int argc, char **argv) {
#ifndef SHIPPING
    puts("Build " __DATE__);
#endif
}
