/*
 * Munkres.h
 *
 *  Created on: Nov 20, 2018
 *      Author: xg590@nyu.edu
 */

#ifndef MUNKRES_H_
#define MUNKRES_H_

void print_vector(int *v, int n);
int munkres(double *cost_matrix, int width, int length, int *matched_col, int *matched_row);

#endif /* MUNKRES_H_ */
