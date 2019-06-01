extern "C" int add(int x, int y);

int x = 0;
int y = 1;
int z = 0;

int main ()
{
    int a = 5;
    int b = 6;
    x = add(a, b);

    return x;
}
