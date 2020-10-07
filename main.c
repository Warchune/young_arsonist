#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#define FILE_IN "./files/f.in"
#define FILE_OUT "./files/f.out"
#define MAX_MATCHES 40    //максимальное количество спичек
#define MAX_VERTEX (MAX_MATCHES * 2)  //максильное количесвтво точек графа
#define NONE_VALUE 10000001L    //несуществующее время сгорания

struct match {  //спичка
    int x1, x2, y1, y2;
    long int time;
};
struct match_list {  //список спичек
    int len;  //количество спичек
    struct match list[MAX_MATCHES];
};
struct vertex {  //вершина графа
    int x, y;
};
struct graph {  //граф
    int count_vertex; //количество вершин в графе
    long int weight[MAX_VERTEX][MAX_VERTEX];  //вес ребр графа
    struct vertex list[MAX_VERTEX];   //список вершин
};
struct result {
    int x, y;
    double time;
};

struct match_list read();

struct graph make_graph(struct match_list);

struct result calculate(struct graph);

void write(struct result);


int main() {
    struct match_list ml;
    struct graph g;
    struct result res;

    ml = read();
    g = make_graph(ml);
    res = calculate(g);

    write(res);

    return 0;
}

struct match_list read() {
    struct match_list ml;
    bool err_res;
    FILE *f_in;
    f_in = fopen(FILE_IN, "r");
    if (f_in) {
        err_res = fscanf(f_in, "%d", &ml.len);
        if (!err_res) {
            printf("Incorrect data format: line(%d)\n", 1);
            exit(1);
        }
        for (int i = 0; i <= ml.len; i++) {
            err_res = fscanf(f_in, "%d%d%d%d%ld", &ml.list[i].x1, &ml.list[i].y1, &ml.list[i].x2, &ml.list[i].y2, &ml.list[i].time);
            if (!err_res){
                printf("Incorrect data format: line(%d)\n", i+1);
                exit(1);
            }
        }
        fclose(f_in);
    } else {
        perror("read: fopen() ");
        exit(errno);
    }

    return ml;
}

int get_vertex(int vx, int vy, struct graph *g) {
    for (int i = 0; i < (*g).count_vertex; i++) {   //проверка на уже существующую вершину
        if (((*g).list[i].x == vx) && ((*g).list[i].y == vy)) {
            return i;
        }
    }
    (*g).list[(*g).count_vertex].x = vx;
    (*g).list[(*g).count_vertex].y = vy;
    for (int i = 0; i != (*g).count_vertex; i++) {  //заполнение несуществующих ребер
        (*g).weight[i][(*g).count_vertex] = NONE_VALUE;
        (*g).weight[(*g).count_vertex][i] = NONE_VALUE;
    }
    (*g).weight[(*g).count_vertex][(*g).count_vertex] = 0;
    (*g).count_vertex++;
    return (*g).count_vertex - 1;

}

void get_weight(int x1, int y1, int x2, int y2, long int time, struct graph *g) {
    (*g).weight[get_vertex(x1, y1, g)][get_vertex(x2, y2, g)] = time;
    (*g).weight[get_vertex(x2, y2, g)][get_vertex(x1, y1, g)] = time;
}

struct graph make_graph(struct match_list d) {
    struct graph g;
    g.count_vertex = 0;
    for (int i = 0; i < d.len; i++) {
        //  умножение координат на 2, соответственно удвоение спичек
        get_weight(d.list[i].x1 * 2, d.list[i].y1 * 2, d.list[i].x1 + d.list[i].x2, d.list[i].y1 + d.list[i].y2,
                   d.list[i].time, &g);
        get_weight(d.list[i].x1 + d.list[i].x2, d.list[i].y1 + d.list[i].y2, d.list[i].x2 * 2, d.list[i].y2 * 2,
                   d.list[i].time, &g);
    }

    return g;
}

double get_time_at(int p, struct graph g, double distance[MAX_VERTEX][MAX_VERTEX]) {
    double current_time = 0;
    double this_edge;
    for (int i = 0; i < g.count_vertex; i++) {
        if (distance[p][i] > current_time) {
            current_time = distance[p][i];
        }
    }
    for (int i = 0; i < g.count_vertex; i++) {
        for (int j = i + 1; j < g.count_vertex; j++) {
            if (g.weight[i][j] < NONE_VALUE) {
                if ((distance[p][i] < (distance[p][j] + (double) g.weight[i][j])) &&
                    (distance[p][j] < (distance[p][i] + (double) g.weight[i][j]))) {
                    if (distance[p][i] < distance[p][j]) {
                        this_edge = distance[p][j] + ((double) g.weight[i][j] - (distance[p][j] - distance[p][i])) / 2;
                    } else {
                        this_edge = distance[p][i] + ((double) g.weight[i][j] - (distance[p][i] - distance[p][j])) / 2;
                    }
                    if (this_edge > current_time) {
                        current_time = this_edge;
                    }
                }
            }
        }
    }
    return current_time;
}

void floyd_alg(double distance[MAX_VERTEX][MAX_VERTEX], int count) {
    for (int k = 0; k < count; k++) {
        for (int i = 0; i < count; i++) {
            if (distance[i][k] < NONE_VALUE) {
                for (int j = 0; j < count; j++) {
                    if (distance[i][k] + distance[k][j] < distance[i][j]) {
                        distance[i][j] = distance[i][k] + distance[k][j];
                    }
                }
            }
        }
    }
    //проверка на связность фигуры
    for (int i = 0; i < count; i++){
        if (distance[0][i] == NONE_VALUE){
            printf("Incorrect data format: incoherent figure\n");
            exit(1);
        }
    }
}

struct result calculate(struct graph g) {
    struct result p;
    double distance[MAX_VERTEX][MAX_VERTEX];
    double current_time;

    for (int i = 0; i < g.count_vertex; i++) {
        for (int j = 0; j < g.count_vertex; j++) {
            distance[i][j] = (double) g.weight[i][j];
        }
    }

    floyd_alg(distance, g.count_vertex);

    p.time = NONE_VALUE;
    for (int i = 0; i < g.count_vertex; i++) {
        if ((g.list[i].x % 2 == 0) && (g.list[i].y % 2 == 0)) {
            current_time = get_time_at(i, g, distance);
            if (current_time < p.time) {
                p.time = current_time;
                p.x = g.list[i].x / 2;
                p.y = g.list[i].y / 2;
            }
        }
    }

    return p;
}

void write(struct result p) {
    FILE *f_out;
    f_out = fopen(FILE_OUT, "w");
    if (f_out) {
        fprintf(f_out, "%d %d\n"
                       "%.2f\n", p.x, p.y, p.time / 2);
        fclose(f_out);
    } else {
        perror("write: fopen() ");
        exit(errno);
    }
}