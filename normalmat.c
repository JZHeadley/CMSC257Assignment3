#include <sys/times.h>
#include<stdlib.h>
#include<stdio.h>

double start, stop, used, mf;

double ftime(void);
void multiply (double **a, double **b, double **c, int n);

double ftime (void){
    struct tms t;
    times ( &t );
    return (t.tms_utime + t.tms_stime) / 100.0;
}

void initializeArrays(double **a, double **b, int n){
    int i,j;
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++)
            a[i][j]=8;
    }

    for (i=0; i<n; i++) {
        for (j=0; j<n; j++)
            b[i][j]=7;
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

void transpose(double **matrixToTranspose, int size){
    int i,j;

    // Initializing array to put transpose in
    double **transpose=(double**)malloc(size*sizeof(double));
    for(i=0;i<size;i++){
        transpose[i]=malloc(size*sizeof(double));
    }
    // Filling it with 0's
    for(i=0;i<size;i++){
        for(j=0; j<size; j++){
            transpose[i][j]=0;
        }
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

void printDifferencesInTime(double endTime ,double startTime, int size){
    double used = endTime - startTime;
    double mf = (size*size * 2.0) / used / 1000000.0 * size;
    printf ("\n");
    printf ( "Elapsed time:   %10.2f \n", used);
    printf ( "DP MFLOPS:       %10.2f \n", mf);
}

void createSomeReport(){
    int i,n,j,m;
    double **a, **b, **c, **transposedMatrixB;

    for(m=0;m<20;m++){
        n=m*100;
        printf("\nn=%i",n);
        //Populate arrays....
        a= (double**)malloc(n*sizeof(double));
        b= (double**)malloc(n*sizeof(double));
        c= (double**)malloc(n*sizeof(double));
        transposedMatrixB= (double**)malloc(n*sizeof(double));

        for (i=0; i<n; i++) {
            a[i]= malloc(sizeof(double)*n);
            b[i]= malloc(sizeof(double)*n);
            c[i]= malloc(sizeof(double)*n);
            transposedMatrixB[i] = malloc(sizeof(double)*n);
        }

        initializeArrays(a,b,n);
        for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                transposedMatrixB[i][j] = b[i][j];
            }
        }
        transpose(transposedMatrixB,n);

        printf("\nNormal Matrix Multiplication\t\t");
        start = ftime();
        multiply (a,b,c,n);

        used = ftime() - start;
        mf = (n*n * 2.0) / used / 1000000.0 * n;
        //printf ("\n");
        printf ( "Elapsed time:   %10.2f \t", used);
        printf ( "DP MFLOPS:       %10.2f \n", mf);

        printf("Transposed Matrix Multiplication\t");
        start = ftime();
        transposeMultiply(a,transposedMatrixB,c,n);
        used = ftime() - start;
        mf = (n*n * 2.0) / used / 1000000.0 * n;
        //printf ("\n");
        printf ( "Elapsed time:   %10.2f \t", used);
        printf ( "DP MFLOPS:       %10.2f \n", mf);

        //        printf("\nBlocked Matrix Multiplication");
        //      start = ftime();
        //    used = ftime() - start;
        //  mf = (n*n * 2.0) / used / 1000000.0 * n;
        //        printf ("\n");
        //      printf ( "Elapsed time:   %10.2f \t", used);
        //    printf ( "DP MFLOPS:       %10.2f \n", mf);


    }
}

int main (void){
    //createSomeReport();
    //return 0;
    int i, j, n;
    double **a, **b, **c, **transposedMatrixB;

    printf ( "Enter the value of n: ");
    scanf ( "%d", &n);

    //Populate arrays....
    a= (double**)malloc(n*sizeof(double));
    b= (double**)malloc(n*sizeof(double));
    c= (double**)malloc(n*sizeof(double));
    transposedMatrixB= (double**)malloc(n*sizeof(double));

    for (i=0; i<n; i++) {
        a[i]= malloc(sizeof(double)*n);
        b[i]= malloc(sizeof(double)*n);
        c[i]= malloc(sizeof(double)*n);
        transposedMatrixB[i] = malloc(sizeof(double)*n);
    }

    initializeArrays(a,b,n);

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            transposedMatrixB[i][j] = b[i][j];
        }
    }

    printf("\nNormal Matrix Multiplication\n");
    start = ftime();
    multiply (a,b,c,n);
    printDifferencesInTime(ftime(),start, n);

    printf("\nTransposed Matrix Multiplication\n");
    transpose(b,n);
    start = ftime();
    transposeMultiply(a,b,c,n);
    printDifferencesInTime(ftime(),start,n);

    printf("\nBlocked Matrix Multiplication\n");

    return (0);
}

