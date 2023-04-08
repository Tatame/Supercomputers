#include <omp.h>
#include <iostream>
// add_compile_options(-O3)
int f(int b){
    for (int i = 1; i < 5e8; i++){
        b = b*1;
    }
    return b;
}

int main() {
    double start = omp_get_wtime();
    int a[100], b[100];
    // Инициализация массива b
    for(int i = 0; i<100; i++) b[i] = i;
    // Директива OpenMP для распараллеливания цикла
    //#pragma omp parallel for
    for(int i = 0; i<100; i++)
    {
        a[i] = f(b[i]);
        b[i] = 2*a[i];
        //std::cout << i << std::endl;
    }

    int result = 0;
    // Далее значения a[i] и b[i] используются, например, так:
    //#pragma omp parallel for reduction(+ : result)
    for(int i = 0; i<100; i++) result += (a[i] + b[i]);
    double end = omp_get_wtime();

    std::cout << "Result = " << result << std::endl;
    std::cout << "Time = " << end - start << std::endl;
    //
    return 0;
}
