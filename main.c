#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>


typedef uint64_t POS;
POS final = 0;
int choices[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int lengths[9] = {5, 384, 336, 96, 168, 96, 96, 96, 96};
POS all_possible_positions[9][384];
int ptr[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

double known_distances[5][6] = {
    {1.0, 1.0, 1.0, 1.732, 2.0, 2.64575},
    {1.0, 1.0, 1.0, 1.0, 1.732, 2.0},
    {1.0, 1.0, 1.0, 1.732, 1.732, 2.64575},
    {1.0, 1.0, 1.0, 1.732, 1.732, 2.0},
    {1.0, 1.0, 1.0, 1.41421, 2.0, 2.23606}
};


double coords[35][3] = {
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {2.0, 0.0, 0.0},
        {3.0, 0.0, 0.0},
        {4.0, 0.0, 0.0},
        {0.5, 0.866, 0.0},
        {1.5, 0.866, 0.0},
        {2.5, 0.866, 0.0},
        {3.5, 0.866, 0.0},
        {1.0, 1.732, 0.0},
        {2.0, 1.732, 0.0},
        {3.0, 1.732, 0.0},
        {1.5, 2.598, 0.0},
        {2.5, 2.598, 0.0},
        {2.0, 3.464, 0.0},
        {0.5, 0.28867, 0.8165},
        {1.5, 0.28867, 0.8165},
        {2.5, 0.28867, 0.8165},
        {3.5, 0.28867, 0.8165},
        {1.0, 1.15467, 0.8165},
        {2.0, 1.15467, 0.8165},
        {3.0, 1.15467, 0.8165},
        {1.5, 2.02067, 0.8165},
        {2.5, 2.02067, 0.8165},
        {2.0, 2.88667, 0.8165},
        {1.0, 0.57734, 1.633},
        {2.0, 0.57734, 1.633},
        {3.0, 0.57734, 1.633},
        {1.5, 1.44334, 1.633},
        {2.5, 1.44334, 1.633},
        {2.0, 2.30934, 1.633},
        {1.5, 0.86601, 2.4495},
        {2.5, 0.86601, 2.4495},
        {2.0, 1.73201, 2.4495},
        {2.0, 1.15468, 3.266}
};


int planes[21][15] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14},
    {15,16,17,18,19,20,21,22,23,24,24,24,24,24,24},
    {25,26,27,28,29,30,30,30,30,30,30,30,30,30,30},
    {0,5,9,12,14,15,19,22,24,25,28,30,31,33,34},
    {1,6,10,13,16,20,23,26,29,32,32,32,32,32,32},
    {2,7,11,17,21,27,27,27,27,27,27,27,27,27,27},
    {0,1,2,3,4,15,16,17,18,25,26,27,31,32,34},
    {5,6,7,8,19,20,21,28,29,33,33,33,33,33,33},
    {9,10,11,22,23,30,30,30,30,30,30,30,30,30,30},
    {4,8,11,13,14,18,21,23,24,27,29,30,32,33,34},
    {3,7,10,12,17,20,22,26,28,31,31,31,31,31,31},
    {2,6,9,16,19,25,25,25,25,25,25,25,25,25,25},
    {5,6,7,8,15,16,17,18,18,18,18,18,18,18,18},
    {9,10,11,19,20,21,25,26,27,27,27,27,27,27,27},
    {12,13,22,23,28,29,31,32,32,32,32,32,32,32,32},
    {1,6,10,13,15,19,22,24,24,24,24,24,24,24,24},
    {2,7,11,16,20,23,25,28,30,30,30,30,30,30,30},
    {3,8,17,21,26,29,31,33,33,33,33,33,33,33,33},
    {3,7,10,12,18,21,23,24,24,24,24,24,24,24,24},
    {2,6,9,17,20,22,27,29,30,30,30,30,30,30,30},
    {1,5,16,19,26,28,32,33,33,33,33,33,33,33,33}
};


POS set3(int i, int j, int k) {
    POS p = 0;
    POS one = 1;
    p |= one << i;
    p |= one << j;
    p |= one << k;
    return p;
}


POS set4(int i, int j, int k, int l) {
    POS p = 0;
    POS one = 1;
    p |= one << i;
    p |= one << j;
    p |= one << k;
    p |= one << l;
    return p;
}


int in_plane(int plane_idx, int i, int j, int k, int l) {
    int found_i = 0;
    int found_j = 0;
    int found_k = 0;
    int found_l = 0;

    for(int n = 0; n < 15; n++) {
        if(planes[plane_idx][n] == i) {
            found_i = 1;
        }
        if(planes[plane_idx][n] == j) {
            found_j = 1;
        }
        if(planes[plane_idx][n] == k) {
            found_k = 1;
        }
        if(planes[plane_idx][n] == l) {
            found_l = 1;
        }
    }
    return found_i && found_j && found_k && found_l;
}


int is_planar(int i, int j, int k, int l) {
    for(int plane_idx = 0; plane_idx < 21; plane_idx++) {
        if(in_plane(plane_idx, i, j, k, l)) {
            return true;
        }
    }
    return false;
}


double distance(int i, int j) {
    double x1 = coords[i][0];
    double y1 = coords[i][1];
    double z1 = coords[i][2];
    double x2 = coords[j][0];
    double y2 = coords[j][1];
    double z2 = coords[j][2];

    double dx = x1 - x2;
    double dy = y1 - y2;
    double dz = z1 - z2;

    double sq = dx*dx + dy*dy + dz*dz;

    return sqrt(sq);
}


void bubble_sort_six(double *d) {
    for(int i = 1; i < 6; i++) {
        for(int j = i; j > 0; j--) {
            if(d[j] < d[j-1]) {
                double tmp = d[j-1];
                d[j-1] = d[j];
                d[j] = tmp;
            }
            else {
                break;
            }
        }
    }
}


int about_equal(double a, double b) {
    a = a - b;
    if(a < 0) {
        a = a * -1;
    }
    return a < 0.01;
}


int is_match(double *a, double *b) {
    for(int i = 0; i < 6; i++) {
        if(!about_equal(a[i], b[i])) {
            return false;
        }
    }
    return true;
}


void match(int i, int j, int k, int l) {
    double distances[6];
    distances[0] = distance(i, j);
    distances[1] = distance(i, k);
    distances[2] = distance(i, l);
    distances[3] = distance(j, k);
    distances[4] = distance(j, l);
    distances[5] = distance(k, l);

    bubble_sort_six(distances);
    if(distances[5] > 2.66) {
        return;
    }

    for(int idx = 0; idx < 5; idx++) {
        if(is_match(distances, known_distances[idx])) {
            if(idx == 4) {
                all_possible_positions[idx+1][ptr[idx+1]] = set4(i, j, k, l); ptr[idx+1]++;
                all_possible_positions[idx+2][ptr[idx+2]] = set4(i, j, k, l); ptr[idx+2]++;
                all_possible_positions[idx+3][ptr[idx+3]] = set4(i, j, k, l); ptr[idx+3]++;
                all_possible_positions[idx+4][ptr[idx+4]] = set4(i, j, k, l); ptr[idx+4]++;
            }
            else {
                all_possible_positions[idx+1][ptr[idx+1]] = set4(i, j, k, l); ptr[idx+1]++;
            }
            return;
        }
    }
}


void precompute() {
    for(int i = 0; i <= 31; i++) {
        for(int j = i+1; j <= 32; j++) {
            for(int k = j+1; k <= 33; k++) {
                for(int l = k+1; l <= 34; l++) {
                    if(is_planar(i, j, k, l)) {
                        match(i, j, k, l);
                    }
                }
            }
        }
    }
}


void initialize() {
    POS one = 1;
    for(int i = 0; i < 35; i++) {
        final = final | (one << i);
    }

    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 384; j++) {
            all_possible_positions[i][j] = 0;
        }
    }

    all_possible_positions[0][0] = set3(0, 1, 2);
    all_possible_positions[0][1] = set3(1, 2, 3);
    all_possible_positions[0][2] = set3(5, 6, 7);
    all_possible_positions[0][3] = set3(9, 10, 11);
    all_possible_positions[0][4] = set3(19, 20, 21);

}


int search(int level, POS prev) {
    if(level == 9) {
        if(prev == final) {
            return true;
        }
        return false;
    }

    for(int index = 0; index < lengths[level]; index++) {
        POS pos = all_possible_positions[level][index];
        if((prev&pos) == 0) {
            if(search(level+1, prev|pos)) {
                choices[level] = index;
                return true;
            }
        }
    }

    return false;
}

int display() {
    int occupied[35] = {};
    POS one = 1;
    POS zero = 0;

    printf("Choices:\n");
    for (int i = 0; i < 9; i++) {
        printf("%d ", choices[i]);
    }
    printf("\n");

    for (int i = 0; i < 9; i++) {
        POS p = all_possible_positions[i][choices[i]];
        for (int j = 0; j < 35; j++) {
            POS mask = one << j;
            if ((p&mask) != zero) {
                occupied[j] = i;
            }
        }
    }

    for (int i = 0; i < 35; i++) {
        printf("%d ", occupied[i]+1);
    }
    printf("\n");
}


int main() {
    initialize();
    precompute();
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &start);
    search(0, 0);
    clock_gettime(CLOCK_REALTIME, &end);

    double start_d = start.tv_sec * 1.0 + start.tv_nsec*1.0/1000000000;
    double end_d = end.tv_sec * 1.0 + end.tv_nsec*1.0/1000000000;
    printf("%f sec\n", end_d - start_d);
    display();
}
