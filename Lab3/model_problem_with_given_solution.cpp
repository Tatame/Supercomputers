#include <iostream>
#include <vector>
#include <cmath>
#include <mpich/mpi.h>

int process_part(int N, int size, int rank);
void init(double* a, int len, double value);
void dot(double* result, int result_len, int start,  double** A, const double* x, int x_len);
void subtraction(double* result, int len, const double* a, const double* b);
void multiplication(double* result, int  len, const double* a, double alpha);
double norma(const double* a, int len);
bool run_criteria(double* tmp, double** A, int A_row_count, int start, double* x, int x_len, double* b, double error);

int main(int argc, char **argv){

    int N = (argc > 1 ? std::stoi(argv[1]) : 10000);
    double error = (argc > 2 ? std::stod(argv[2]) : 10e-6);
    double rate = 1.0 / (N * 2);

    int errCode;
    if ((errCode = MPI_Init(&argc, &argv)) != 0)
    {
        return errCode;
    }
    MPI_Bcast(&rate, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&error, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // A initialization
    auto** A = new double* [N];
    for (int i = 0; i < N; ++i){
        A[i] = new double [N];
    }
    for (int i = 0; i < N; ++i){
        for (int j = 0; j < N; ++j) {
            if (i == j) A[i][j] = 2;
            else A[i][j] = 1;
        }
    }

    int size;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int local_size = process_part(N, size, rank);
    // Initializing vectors
    auto* local_x = new double [local_size];
    init(local_x, local_size, 0);
    auto* result_x = new double [N];
    init(result_x, N, 0);

    auto* local_b = new double [local_size];
    init(local_b, local_size, N+1);
    auto* b = new double [N];
    init(b, N, N+1);

    auto* local_tmp = new double [local_size];
    init(local_tmp, local_size, 0.0);
    auto* tmp = new double [N];
    init(tmp, N, 0.0);

    auto* process_local_starts = new int [size];
    auto* process_local_sizes = new int [size];

    for (int i = 0; i < size; ++i){
        process_local_sizes[i] = process_part(N, size, i);
        if (i == 0) process_local_starts[i] = 0;
        else process_local_starts[i] = process_local_starts[i-1] + process_local_sizes[i-1];
    }
    if (rank == 0){
        for (int i = 0; i < size; ++i) {
            std::cout << process_local_sizes[i] << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < size; ++i) {
            std::cout << process_local_starts[i] << " ";
        }
        std::cout << std::endl;
        std::cout << size << std::endl;
    }

    double start = MPI_Wtime();

    while (run_criteria(tmp, A, N, 0, result_x, N, b, error)){
        dot(local_tmp, local_size, process_local_starts[rank], A, result_x, N);
        subtraction(local_tmp, local_size, local_tmp, local_b);
        multiplication(local_tmp, local_size, local_tmp, rate);
        subtraction(local_x, local_size, local_x, local_tmp);
        //,MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allgatherv(local_x, local_size, MPI_DOUBLE, result_x, process_local_sizes,
                       process_local_starts, MPI_DOUBLE, MPI_COMM_WORLD);
        if (rank == 0) std::cout << result_x[0] << std::endl;

    }

    double finish = MPI_Wtime();
    if (rank == 0) std::cout << finish - start << std::endl;

    MPI_Finalize();

    for (int i=0; i<N; ++i){
        if ((1.0 - result_x[i]) > 10e-5) std::cout << "False" << std::endl;
    }

    return 0;
}

int process_part(int N, int size, int rank){
    // Calculates the size of the local vector
    return N / size + (rank < (N % size));
}

void init(double* a, int len, double value){
    // Initializes a vector with a single number
    for (int i=0; i < len; ++i){
        a[i] = value;
    }
}

bool run_criteria(double* tmp, double** A, int A_row_count, int start, double* x, int x_len, double* b, double error){
    // Checks the cycle continuation criterion
    double norm = 0;
    init(tmp, A_row_count, 0.0);
    dot(tmp, A_row_count, start, A, x, x_len);
    subtraction(tmp, A_row_count, tmp, b);
    norm = norma(tmp, A_row_count);
    norm = norm / norma(b, A_row_count);
    if (norm > error) return true;
    else return false;
}

void dot(double* result, int result_len, int start, double** A, const double* x, int x_len){
    for (int i = start; i < start + result_len; ++i){
        for (int j = 0; j < x_len; ++j) {
            result[i-start] += A[i][j] * x[j];
        }
    }
}

void subtraction(double* result, int len, const double* a, const double* b){
    for (int i=0; i < len; ++i){
        result[i] = a[i] - b[i];
    }
}

void multiplication(double* result, int  len, const double* a, double alpha){
    for (int i=0; i < len; ++i){
        result[i] = alpha * a[i];
    }
}

double norma(const double* a, int len){
    double norm = 0.0;
    for (int i=0; i<len; ++i){
        norm += a[i] * a[i];
    }
    return sqrt(norm);
}