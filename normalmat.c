#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/times.h>

#define RED     "\x2b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


double start, stop, used, mf;

sem_t *semaphores;
double *sharedA, *sharedB,  *sharedC;

double ftime(void);
void multiply (double **a, double **b, double **c, int n);
void createSomeReport();

double ftime (void){
    struct tms t;
    times ( &t );
    return (t.tms_utime + t.tms_stime) / 100.0;
}

void initializeArrays(double **a, double **b, int n){
    int i,j;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++)
            a[i][j]=i+j;
    }

    for (i=0; i<n; i++) {
        for (j=0; j<n; j++)
            b[i][j]=rand()%100 +1;
    }

}

void print1DMatrix(double *a,int size){
    int row,column;
    if(size<=32){
        for(row=0;row<size;row++){
            for(column=0;column<size;column++){
                printf("%10.0f\t",a[row * size + column]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void printmatrix(double **a,int size){
    int row,column;
    if(size<=32){
        for(row=0;row<size;row++){
            for(column=0;column<size;column++){
                printf("%10.0f\t",a[row][column]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void sharedMultiply (double *a, double *b, double *c, int n){
    printf("SharedA\n");
    print1DMatrix(a,n);
    printf("SharedB\n");
    print1DMatrix(b,n);
    printf("SharedC\n");
    print1DMatrix(c,n);
    int i, j, k;
    for (i=0; i<n; i++){
        for (j=0; j<n; j++)
            c[i*n+j] = 0;
    }
    for (i=0; i<n; i++){
        for (j=0; j<n; j++){
            for (k=0; k<n; k++)
                c[i*n+j]= c[i*n+j] + a[i*n+k] * b[k*n+j];
        }
    }
}

void multiply (double **a, double **b, double **c, int n){
    int i, j, k;
    for (i=0; i<n; i++){
        for (j=0; j<n; j++)
            c[i][j] = 0;
    }
    for (i=0; i<n; i++){
        for (j=0; j<n; j++){
            for (k=0; k<n; k++)
                c[i][j]= c[i][j] + a[i][k] * b[k][j];
        }
    }
}

void transpose(double **matrixToTranspose, int size) {
    int i,j;
    // Initializing array to put transpose in
    double **transpose=(double**)malloc(size*sizeof(double));
    for(i=0;i<size;i++){
        transpose[i]=((double*) (malloc(size * sizeof(double))));
    }

    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            transpose[j][i] = matrixToTranspose[i][j];
        }
    }
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            matrixToTranspose[i][j] = transpose[i][j];
        }
    }
    free(transpose);
}

void transposeMultiply(double **a, double **transposedMatrix, double **output, int size){
    int i,j,k;
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            output[i][j] = 0;
        }
    }
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            for(k=0;k<size;k++){
                output[i][j] = output[i][j] + a[i][k] * transposedMatrix[j][k];
            }
        }
    }

}

int min(int i, int n){
    if(i>n){
        return n;
    }
    return i;
}

void blockedMultiply(double **a, double **b, double **output, int size, int blockSize){
    int i, j, k, l, m, n, imin=0, jmin=0, kmin=0;
    for(i=0; i < size; i += blockSize){
        imin = min(i + blockSize - 1, size);
        for(j=0; j < size; j += blockSize){
            jmin = min(j + blockSize - 1, size);
            for(k=0; k < size; k += blockSize){
                kmin = min( k + blockSize - 1, size);
                for (l=i; l < imin; l++){
                    for(m=j; m < jmin; m++){
                        for(n=k; n < kmin; n++){
                            output[i][j] = output[i][j] + a[i][k] * b[k][j];
                        }
                    }
                }
            }
        }
    }
}


void setupSharedMemForThreads(int size) {
    int shmfd;

    shmfd = shm_open("/jzheadleySemaphores", O_RDWR | O_CREAT, 0666);
    ftruncate(shmfd, size * size * sizeof(sem_t));
    semaphores = (sem_t *)mmap (NULL, size*size*sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    close(shmfd);
    shm_unlink("/jzheadleySemaphores");

    shmfd = shm_open("/jzheadleySharedA", O_RDWR | O_CREAT, 0666);
    ftruncate(shmfd, size * size * sizeof(double));
    sharedA = (double *)mmap (NULL, size * size * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    close(shmfd);
    shm_unlink("/jzheadleySharedA");

    shmfd = shm_open("/jzheadleySharedB", O_RDWR | O_CREAT, 0666);
    ftruncate(shmfd, size * size * sizeof(double));
    sharedB = (double *)mmap (NULL, size*size*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    close(shmfd);
    shm_unlink("/jzheadleySharedB");

    shmfd = shm_open("/jzheadleySharedC", O_RDWR | O_CREAT, 0666);
    ftruncate(shmfd, size * size * sizeof(double));
    sharedC = (double *)mmap (NULL, size*size*sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0);
    close(shmfd);
    shm_unlink("/jzheadleySharedC");

}

void threadedBlockMultiply(double **a, double **b, double **output, int size, int blockSize) {

}

void printDifferencesInTime(double endTime ,double startTime, int size){
    double used = endTime - startTime;
    double mf = (size*size * 2.0) / used / 1000000.0 * size;
    printf ("\n");
    printf ( "Elapsed time:   %10.2f \n", used);
    printf ( "DP MFLOPS:       %10.2f \n" RESET, mf);
}


int main (int argc, char *argv[]){
    //createSomeReport();
    //return 0;
    int i, j, n, blockSize;
    double **a, **b, **c, **transposedMatrixB;

    if(argc == 3){
        printf("Command Line args were passed");
        n = atoi(argv[1]);
        blockSize = atoi(argv[2]);
    } else{
        printf ( "Enter the value of n: ");
        scanf ( "%d", &n);
        printf( "Enter the value for blockSize: " );
        scanf ( "%d", &blockSize );
    }
    setupSharedMemForThreads(n);
    //Populate arrays....
    a = (double**)malloc(n*sizeof(double));
    b = (double**)malloc(n*sizeof(double));
    c = (double**)malloc(n*sizeof(double));
    transposedMatrixB= (double**)malloc(n*sizeof(double));

    for (i=0; i<n; i++) {
        a[i]= (double*)malloc(sizeof(double)*n);
        b[i]= (double*)malloc(sizeof(double)*n);
        c[i]= (double*)malloc(sizeof(double)*n);
        transposedMatrixB[i] = (double*)malloc(sizeof(double)*n);
    }

    initializeArrays(a,b,n);

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            transposedMatrixB[i][j] = b[i][j];
        }
    }

    printf(GREEN"\nNormal Matrix Multiplication\n");
    start = ftime();
    multiply (a,b,c,n);
    printDifferencesInTime(ftime(),start, n);

    if(n<32){
        printf("Result of normal mult\n");
        printmatrix(c,n);

    }
    for(i=0; i < n; i++){
        for(j=0; j < n; j++){
            sharedA[i*n+j]= a[i][j];
            sharedB[i*n+j]= b[i][j];
            sharedC[i*n+j]= c[i][j];
        }
    }

    sharedMultiply(sharedA,sharedB,sharedC,n);


    printf(MAGENTA"\nTransposed Matrix Multiplication\n");
    transpose(b,n);
    start = ftime();
    transposeMultiply(a,b,c,n);
    printDifferencesInTime(ftime(),start,n);

    printf(CYAN"\nBlocked Matrix Multiplication\n");
    start = ftime();
    blockedMultiply(a,b,c,n,blockSize);
    printDifferencesInTime(ftime(),start,n);

    printf(YELLOW"\nThreaded Blocked Matrix Multiplication\n");
    start = ftime();
    threadedBlockMultiply(a,b,c,n,blockSize);
    printDifferencesInTime(ftime(),start,n);



    if(n<64){
        printf("A Matrix\n");
        printmatrix(a,n);
        printf("B Matrix\n");
        printmatrix(b,n);
        printf("Transposed matrix\n");
        printmatrix(transposedMatrixB,n);
        printf("Result of transposed multiplication\n");
        printmatrix(c,n);
    } else{
        printf("Enter a matrix with size of less than 32 in order to see the resulting matrix printed\n");
    }

    return (0);
}

void createSomeReport(){
    int i,n,j,m,z;
    double **a, **b, **c, **transposedMatrixB;

    for(m=0;m<=50;m++){
        n=m*100;
        printf("\nn=%i",n);
        //Populate arrays....
        a= (double**)malloc(n*sizeof(double));
        b= (double**)malloc(n*sizeof(double));
        c= (double**)malloc(n*sizeof(double));
        transposedMatrixB= (double**)malloc(n*sizeof(double));

        for (i=0; i<n; i++) {
            a[i]= (double *) malloc(sizeof(double)*n);
            b[i]= (double *)malloc(sizeof(double)*n);
            c[i]= (double *)malloc(sizeof(double)*n);
            transposedMatrixB[i] = (double *)malloc(sizeof(double)*n);
        }

        initializeArrays(a,b,n);
        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                transposedMatrixB[i][j] = b[i][j];
            }
        }
        transpose(transposedMatrixB,n);

        printf(GREEN"\nNormal Matrix Multiplication\t\t");
        start = ftime();
        multiply (a,b,c,n);

        used = ftime() - start;
        mf = (n*n * 2.0) / used / 1000000.0 * n;
        //printf ("\n");
        printf ( "Elapsed time:   %10.2f \t", used);
        printf ( "DP MFLOPS:       %10.2f \n"RESET, mf);

        printf(MAGENTA"Transposed Matrix Multiplication\t");
        start = ftime();
        transposeMultiply(a,transposedMatrixB,c,n);
        used = ftime() - start;
        mf = (n*n * 2.0) / used / 1000000.0 * n;
        //printf ("\n");
        printf ( "Elapsed time:   %10.2f \t", used);
        printf ( "DP MFLOPS:       %10.2f \n"RESET, mf);

        for(z=100; z <= n; z+=100){
            printf(CYAN"\nSize of %i and blockSize of %i", n, z);
            printf("\nBlocked Matrix Multiplication\t\t");
            start = ftime();
            blockedMultiply(a,b,c,n,z);
            used = ftime() - start;
            mf = (n*n * 2.0) / used / 1000000.0 * n;
            //printf ("\n");
            printf ( "Elapsed time:   %10.2f \t", used);
            printf ( "DP MFLOPS:       %10.2f\n"RESET, mf);
        }

    }
}
