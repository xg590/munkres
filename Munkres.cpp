//============================================================================
// Author      : Xiaokang Guo, Nov 17 2018, xg590@nyu.edu.
// Copyright   : You are free to do anything about it
// Description : This is a pure C version of Dr. Robert A. Pilgrim's C# imple-
//				 mentation. His original C# code is at:
// 				 http://csclab.murraystate.edu/~bob.pilgrim/445/munkres.html
//               Minor modification and more notes are made in C code.
//				 A PDF which details Robert's example is also provided at:
//
//============================================================================

/*  This is from Robert A. Pilgrim
    The MIT License (MIT)

    Copyright (c) 2000 Robert A. Pilgrim
                       Murray State University
                       Dept. of Computer Science & Information Systems
                       Murray,Kentucky

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

 */

#include "stdio.h"
#include "string.h"
#include "float.h"

void print_cost_matrix(double *cost, int width, int length) // {cost: pointer to cost matrix, width: row number of matrix, length: column number of matrix}
{
	printf("Cost:");
	for (int i=0; i<width; ++i)
	{
		for (int j=0; j<length; ++j)
			printf("\t%.1lf", *(cost++));
		printf("\n");
	}
}

void print_mask_matrix(int *mask, int width, int length) // {mask: pointer to mask matrix}
{
	printf("Mask:");
	for (int i=0; i<width; ++i)
	{
		for (int j=0; j<length; ++j)
			printf("\t%d", *(mask++));
		printf("\n");
	}
}

void print_vector(int *v, int n) // {v: pointer to vector, n: length of vector}
{
	for (int i=0; i<n; ++i)
		printf("\t%d", *(v++));
	printf("\n");
}

void step_one(double *cost, int width, int length)
{
	double min = 0;
	for (int i=0; i<width; ++i)
	{
		min = *(cost + i*length);
		for (int j=1; j<length; ++j)
			if (*(cost + i*length + j) < min) // Find the minimum in i-th row
				min = *(cost + i*length + j);
		for (int j=0; j<length; ++j)
			*(cost + i*length + j) -= min; // Minimum subtraction
	}
}

void step_two(double *cost, int *mask, int width, int length)
{
	int row_cover[width] = {0};
	int col_cover[length] = {0};
	for (int i=0; i<width; ++i)
	{
		for (int j=0; j<length; ++j)
		{
			if (*(cost+i*length+j) == 0 && *(row_cover+i) == 0 && *(col_cover+j) == 0) // Find an uncovered zero in cost matrix
			{
				*(mask+i*length+j) = 1;  // Generate the mask matrix, star zeros (put a 1 in mask matrix).
				*(row_cover+i) = 1; // Cover the row temporarily
				*(col_cover+j) = 1;
			}
		}
	}
}

int step_three(int *mask, int width, int length, int *row_cover, int *col_cover) // {row_cover: a list contains indexes of the covered rows}
{
	int count = 0;
	for (int i=0; i<width; ++i)
		for (int j=0; j<length; ++j)
			if (*(mask+i*length+j) == 1) // Cover j-th column which contains a starred zero
				*(col_cover+j) = 1;

	for (int i=0; i<length; ++i)
		if (*(col_cover+i) == 1) // Count covered column
			++count;

	if (count >= length)
		return 7; // All columns are covered, then done.
	else
		return 4;
}

void find_a_noncovered_zero(int *row, int *col, // return row and column indexes of the non-covered zero
				 double *cost, int width, int length,
				 int *row_cover, int *col_cover)
{
    *row = -1;
    *col = -1;

	for (int i=0; i<width; ++i)
	{
		for (int j=0; j<length; ++j)
		{
            if (*(cost + i*length + j) == 0 && *(row_cover+i) == 0 && *(col_cover+j) == 0)
            {
                *row = i;
                *col = j;
                return;
            }
		}
    }
}

// methods to support step 4
int find_star_in_row(int *mask, int row, int length) // return the column index of a stared zero in the same row of the newly primed zero
{
	int col = -1;
    for (int j = 0; j < length; ++j)
        if (*(mask+row*length+j) == 1)
            col = j;
    return col;
}

int step_four(double *cost, int *mask, int width, int length, int *row_cover, int *col_cover, int *path_row_0, int *path_col_0) // {path_row_0: row index of the starting point of the zig-zag path in step 5}
{
	int row = -1;
	int col = -1;
	int col_buff = -1;
	while (true)
	{
		find_a_noncovered_zero(&row, &col, cost, width, length, row_cover, col_cover);
		printf("non-covered zero: (%d, %d)\n", row, col);
		if (row == -1)
		{ // no non-covered zeros left
			return 6;
		}
		else
		{ // found a non-covered zero,
			*(mask+row*length+col) = 2;  // Prime the non-covered zero in the mask matrix (put a 2 in mask matrix)
			col_buff = find_star_in_row(mask, row, length); // Find a starred zero in the same row
			if (col_buff > -1) // found an starred zero
			{ // Cover this row and uncover the column containing the starred zero
				*(row_cover+row) = 1;
				*(col_cover+col_buff) = 0;
			}
			else
			{ // Once there is no starred zero in the row containing this primed zero, Go to Step 5.
				*path_row_0 = row; // A starting point for a zig-zag path covering primed and starred zeros
				*path_col_0 = col;
				return 5;
			}
		}
	}
}

// methods to support step 5
int find_star_in_col(int *mask, int col, int length) // return row index
{
    for (int i = 0; i < length; ++i)
        if (*(mask+i*length+col) == 1)
            return i;
    return -1;
}

int find_prime_in_row(int *mask, int row, int length)
{
    for (int j = 0; j < length; ++j)
        if (*(mask+row*length+j) == 2)
            return j;
    return -1;
}

void augment_path(int *mask, int length, int *path, int count) // primed to starred and starred to unmarked
{									     // { path: a list shows the zig-zag path contains primed and starred zeros, path: how many zeros in the path}
	int path_i_0 = 0;
	int path_i_1 = 0;

    for (int i = 0; i < count; ++i)
    {
    	path_i_0 = *(path + i*2);
    	path_i_1 = *(path + i*2 + 1);
        if (*(mask + path_i_0*length + path_i_1) == 1)
        	*(mask + path_i_0*length + path_i_1) = 0;
        else
        	*(mask + path_i_0*length + path_i_1) = 1;
    }
}

void clear_covers(int width, int length, int *row_cover, int *col_cover)
{
    for (int i = 0; i < width; ++i)
    {
        *(row_cover+i) = 0;
    }
        for (int i = 0; i < length; ++i)
        {
        *(col_cover+i) = 0;
    }
}

void erase_primes(int *mask, int width, int length)
{
	for (int i=0; i<width; ++i)
		for (int j=0; j<length; ++j)
        	if (*(mask+i*length+j) == 2)
        		*(mask+i*length+j) = 0;
}

int step_five(int *mask, int width, int length, int *row_cover, int *col_cover, int *path_row_0, int *path_col_0)
{
    bool done = false;
    int row = -1;
    int col = -1;
    int count = 1;
	int path[width+length-1] = {0};
	*(path + count*2 - 2) = *path_row_0; // The result from step 4 becomes the first element in a list called path (starting point in path)
	*(path + count*2 - 1) = *path_col_0;
	printf("starting zero for the zig-zag path: (%d, %d)\n", *path_row_0, *path_col_0);
    while (!done)
    {
    	row = find_star_in_col(mask, *(path + count*2 - 1), length); // Is a starred zero within the column having the starting zero or primed zero (found in following 11th lines)
        if (row > -1)
        { // yes and put the new starred zero in the list
            ++count;
            *(path + count*2 - 2) = row;
            *(path + count*2 - 1) = *(path + count*2 - 3);
        }
        else
        { // no
            break;
        }
        col = find_prime_in_row(mask, *(path + count*2 - 2), length); // Find the primed zero within the row of starred zero(, which will be tested for a new starred zero above)
        ++count;
        *(path + count*2 - 2) = *(path + count*2 - 4);
        *(path + count*2 - 1) = col;
    }
    augment_path(mask, length, path, count);
    clear_covers(width, length, row_cover, col_cover);
    erase_primes(mask, width, length);
    return 3;
}

//methods to support step 6
double find_smallest(double *cost, int width, int length, int *row_cover, int *col_cover) // smallest non-covered number
{
	double min = DBL_MAX;
    for (int i = 0; i < width; ++i)
        for (int j = 0; j < length; ++j)
            if (*(row_cover+i) == 0 && *(col_cover+j) == 0)
                if (min > *(cost + i*length + j))
                    min = *(cost + i*length + j);
    return min;
}

int step_six(double *cost, int width, int length, int *row_cover, int *col_cover)
{
	double min = find_smallest(cost, width, length, row_cover, col_cover); // smallest non-covered number
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < length; ++j)
        {
            if (*(row_cover+i) == 1) // covered row unaffected
            	*(cost + i*length + j) += min;
            if (*(col_cover+j) == 0)
            	*(cost + i*length + j) -= min; // affect the non-covered column
        }
    }
    printf("zero maker works for the noncovered\n");
    return 4;
}

double total_cost(double *cost, int *mask, int width, int length)
{
	double sum = 0;
	for (int i=0; i<width; ++i)
		for (int j=0; j<length; ++j)
        	if (*(mask+i*length+j) == 1)
        		sum += *(cost+i*length+j);
	return sum;
}
int main() {
//	//  ----------------------------------------
//	//  Original test From Dr. Robert A. Pilgrim http://csclab.murraystate.edu/~bob.pilgrim/445/munkres.html
		int width = 3;
		int length = 3;
		double cost_matrix[width * length] =
									 {1, 2, 3,
									  2, 4, 6,
									  3, 6, 9};
//	// --------------------------------------------------------------------
//	//  Test of the example in https://www.wikihow.com/Use-the-Hungarian-Algorithm
//	int width = 5;
//	int length = 4;
//	double cost_matrix[width * length] =
//							   {10, 19, 8, 15,
//								10, 18, 7, 17,
//								13, 16, 9, 14,
//								12, 19, 8, 18,
//								14, 17,10, 19};
//	// --------------------------------------------------------------------
//	//  More tests, for fun
//		double cost_matrix[] = { 1,  9, 23,  4,
//							     6, 10, 11,  2,
//							     5,  4, 33, 11,
//							    10, 11,  9, 10};
//	// --------------------------------------------------------------------
//		double cost_matrix[] = { 1,  6,  5, 10,
//							     9, 10,  4, 11,
//							    23, 11, 33,  9,
//							     4,  2, 11, 10};
//	// -------------------------------------------------
//		double cost_matrix[] = { 1.1,  9.1, 23.2,  4.1,
//							     6.1, 10.1, 11.1,  2.1,
//							     4.1,  6.1, 33.1, 11.1,
//							    10.1,  9.1, 11.1, 10.1};
//	// -------------------------------------------------
//		double cost_matrix[] = {   0,    0,    0,    0,
//	   	   	   	   	   	   	   0,    1,    1,    0,
//	   	   	   	   	   	   	   0,    1,    1,    0,
//	   	   	   	   	   	   	   0,    0,    0,    0};
//	// -----------------------------------------------------------------------
//	//  Test example from https://github.com/hrldcpr/hungarian
//		int edge_length = 8;
//		double cost_matrix[] = {1000,   2,  11,  10,   9,   7,   6,   5, // #0
//		                           6,1000,   1,  8,    8,   4,   6,   7, // #1
//		                           5,  12,1000,  11,   8,  12,   3,  11, // #2
//		                          11,   9,  10,1000,   1,   9,   8,  10, // #3
//		                          11,  11,   9,   4,1000,   2,  10,   9, // #4
//		                          12,   8,   5,   2,  11,1000,  11,   9, // #5
//		                          10,  11,  12,  10,   9,  12,1000,   3, // #6
//		                          10,  10,  10,  10,   6,   3,   1,1000};// #7
//	 //	                           0    1    2    3    4    5    6    7
//	 // -----------------------------------------------------------------------

	double cost_matrix_copy[width * length];
	memcpy(cost_matrix_copy, cost_matrix, sizeof(double)*width*length);
	int mask_matrix[width * length] = {0};
	int row_cover[width] = {0};
	int col_cover[length] = {0};

	printf("Step 1 begins...\n");
	step_one(cost_matrix, width, length);
	printf("Step 1 ends, step 2 next...\nStep 2 begins...\n");
	step_two(cost_matrix, mask_matrix, width, length);
	printf("Step 2 ends, step 3 next...\n");
	int step = 3;
    bool done = false;
	int path_row_0;
	int path_col_0;
    while (!done)
    {
    	print_cost_matrix(cost_matrix, width, length);printf("\n");
    	print_mask_matrix(mask_matrix, width, length);printf("\n");
    	printf("row_cover");print_vector(row_cover, width);
    	printf("col_cover");print_vector(col_cover, length);
        switch (step)
        {
            case 3:
            	printf("-----------------------\nStep 3 begins...\n");
            	step = step_three(mask_matrix, width, length, row_cover, col_cover);
            	printf("Step 3 ends, step %d next...\n\n", step);
                break;
            case 4:
            	printf("-----------------------\nStep 4 begins...\n");
            	step = step_four(cost_matrix, mask_matrix, width, length, row_cover, col_cover, &path_row_0, &path_col_0);
            	printf("Step 4 ends, step %d next...\n\n", step);
                break;
            case 5:
            	printf("-----------------------\nStep 5 begins...\n");
            	step = step_five(mask_matrix, width, length, row_cover, col_cover, &path_row_0, &path_col_0);
            	printf("Step 5 ends, step %d next...\n\n", step);
                break;
            case 6:
            	printf("-----------------------\nStep 6 begins...\n");
            	step = step_six(cost_matrix, width, length, row_cover, col_cover);
            	printf("Step 6 ends, step %d next...\n\n", step);
                break;
            case 7:
                done = true;
                double result = total_cost(cost_matrix_copy, mask_matrix, width, length);
                printf("-----------------------\nMinimum total cost is %.2lf.\n", result);
                printf("The End ~");
                break;
        }
    }
}
