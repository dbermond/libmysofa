/*
 * neighbor.c
 *
 *  Created on: 17.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mysofa_export.h"
#include "mysofa.h"
#include "tools.h"

MYSOFA_EXPORT struct MYSOFA_NEIGHBORHOOD *mysofa_neighborhood_init(struct MYSOFA_HRTF *hrtf,
								   struct MYSOFA_LOOKUP *lookup) {
	int i, index;
	float *origin, *test;
	float radius, radius2;
	float theta, theta2;
	float phi, phi2;

	struct MYSOFA_NEIGHBORHOOD *neighbor = malloc(
		sizeof(struct MYSOFA_NEIGHBORHOOD));
	if (!neighbor)
		return NULL;

	neighbor->elements = hrtf->M;
	neighbor->index = malloc(sizeof(int) * neighbor->elements * 6);
	if (!neighbor->index) {
		free(neighbor);
		return NULL;
	}
	for (i = 0; i < neighbor->elements * 6; i++)
		neighbor->index[i] = -1;

	origin = malloc(sizeof(float)*hrtf->C);
	test = malloc(sizeof(float)*hrtf->C);

	for (i = 0; i < hrtf->M; i ++) {
		memcpy(origin, hrtf->SourcePosition.values + i * hrtf->C, sizeof(float) * hrtf->C);
		convertCartesianToSpherical(origin, hrtf->C);

		phi = 0.5;
		do {
			phi2 = test[0] = origin[0] + phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 0] = index;
				break;
			}
			phi += 0.5;
		} while (phi2 <= lookup->phi_max);

		phi = -0.5;
		do {
			phi2 = test[0] = origin[0] + phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test,3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 1] = index;
				break;
			}
			phi -= 0.5;
		} while (phi2 >= lookup->phi_min);

		theta = 0.5;
		do {
			test[0] = origin[0];
			theta2 = test[1] = origin[1] + theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 2] = index;
				break;
			}
			theta += 0.5;
		} while (theta2 <= lookup->theta_max);

		theta = -0.5;
		do {
			test[0] = origin[0];
			theta2 = test[1] = origin[1] + theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 3] = index;
				break;
			}
			theta -= 0.5;
		} while (theta2 >= lookup->theta_min);

		radius = 0.01;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] + radius;
			convertSphericalToCartesian(test, 3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 4] = index;
				break;
			}
			radius += 0.01;
		} while (radius2 <= lookup->radius_max);

		radius = -0.01;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] + radius;
			convertSphericalToCartesian(test, 3);
			index = mysofa_lookup(lookup, test);
			if (index != i) {
				neighbor->index[i * 6 + 5] = index;
				break;
			}
			radius -= 0.01;
		} while (radius2 >= lookup->radius_min);
	}
	free(test);
	free(origin);
	return neighbor;
}

MYSOFA_EXPORT int* mysofa_neighborhood(struct MYSOFA_NEIGHBORHOOD *neighborhood, int index) {
	if (index < 0 || index >= neighborhood->elements)
		return NULL ;
	return neighborhood->index + index * 6;
}

MYSOFA_EXPORT void mysofa_neighborhood_free(struct MYSOFA_NEIGHBORHOOD *neighborhood) {
	if(neighborhood) {
		free(neighborhood->index);
		free(neighborhood);
	}
}

