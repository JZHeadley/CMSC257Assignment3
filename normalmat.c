#include <sys/times.h>
#include<stdlib.h>
#include<stdio.h>

double start, stop, used, mf;

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

void printmatrix(double **a,int size){
    int row,column;
    if(size<=15){
        for(row=0;row<size;row++){
            for(column=0;column<size;column++){
                printf("%10.0f\t",a[row][column]);
            }
            printf("\n");
        }
        printf("\n");
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
        transpose[i]=malloc(size * sizeof(double));
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
void blockedMultiply(double **a, double **b, double **output, int size){
    int i,j,k,l,m,n;
    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            for(k=0;k<size;k++){

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


int main (int argc, char *argv[]){
    //createSomeReport();
    //return 0;
    int i, j, n, blockSize;
    double **a, **b, **c, **transposedMatrixB;
    if(argc == 3){
        printf("Command Line args were passed");
        n =atoi(argv[1]);
        blockSize = atoi(argv[2]);
    } else{
        printf ( "Enter the value of n: ");
        scanf ( "%d", &n);
        printf( "Enter the value for blockSize: " );
        scanf ( "%d", &blockSize );
    }
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
    printf("Result of normal mult\n");
    printmatrix(c,n);

    printf("\nTransposed Matrix Multiplication\n");
    transpose(b,n);
    start = ftime();
    transposeMultiply(a,b,c,n);
    printDifferencesInTime(ftime(),start,n);

    printf("\nBlocked Matrix Multiplication\n");
    printf("A Matrix\n");
    printmatrix(a,n);
    printf("B Matrix\n");
    printmatrix(b,n);
    printf("Result of transposed multiplication\n");
    printmatrix(c,n);
    printf("Transposed matrix\n");
    printmatrix(transposedMatrixB,n);
    return (0);
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
