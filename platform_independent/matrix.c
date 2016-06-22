#ifndef PI_MATRIX_C
#define PI_MATRIX_C

#include "matrix.h"
#include "macros.h"
#include <malloc.h>

// This is a leaking nightmare
matrix MatrixCreate(int rows, int cols)
{
    matrix newMatrix;
    newMatrix.rows = rows;
    newMatrix.cols = cols;
    newMatrix.elements = (double *)malloc(sizeof(double) * rows * cols);
    return newMatrix;
}

void MatrixMultiply(matrix *a, matrix *b, matrix *resultMatrix)
{
    ASSERT(a->cols == b->rows);
    ASSERT(resultMatrix->rows == a->rows);
    ASSERT(resultMatrix->cols == b->cols);

    for (int rows = 0; rows < a->rows; ++rows)
    {
        for (int cols = 0; cols < b->cols; ++cols)
        {
            double result = 0;
            for (int c = 0; c < a->cols; ++c)
            {
                result += a->elements[a->cols * rows + c] *
                          b->elements[b->cols * c + cols];
            }

            resultMatrix->elements[resultMatrix->cols * rows + cols] = result;
        }
    }
}


#endif
