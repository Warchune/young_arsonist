#include "young_arsonist.h"

#define MAX_NG (MAX_N * 2)
#define NONE_VALUE 10^7+1L

int ng; //количество точек в графе
long int edge[MAX_NG][MAX_NG];  //ребра графа
double distance[MAX_NG][MAX_NG];    //результат выполнения алгортима Флойда
struct {
    int x, y;
} vertex[MAX_NG];   //пронумерованные вершины

int get_vertex(int vx, int vy){
    for (int i = 0; i < ng; i++){   //проверка на уже существующую вершину
        if((vertex[i].x == vx) && (vertex[i].y == vy)) {
            return i;
        }
    }
    vertex[ng].x = vx;
    vertex[ng].y = vy;
    for (int i = 0; i != ng; i++){  //заполнение несуществующих ребер
        edge[i][ng] = NONE_VALUE;
        edge[ng][i] = NONE_VALUE;
    }
    edge[ng][ng] = 0;
    ng++;
    return ng - 1;
}

void add_edge(int x1, int y1, int x2,int  y2, long int time){
    edge[get_vertex(x1, y1)][get_vertex(x2, y2)] = time;
    edge[get_vertex(x2, y2)][get_vertex(x1, y1)] = time;
}

void build_graph(){
    ng = 0;
    for (int i = 0; i < n; i++){
        //  умножение координат на 2, соответственно удвоение спичек
        add_edge(match[i].x1 * 2, match[i].y1 * 2, match[i].x1 + match[i].x2, match[i].y1 + match[i].y2, match[i].time);
        add_edge(match[i].x1 + match[i].x2, match[i].y1 + match[i].y2, match[i].x2 * 2, match[i].y2 * 2, match[i].time);
    }
}

void find_shortest_paths(){
    for (int i = 0; i < ng; i++){
        for (int j = 0; j < ng; j++){
            distance[i][j] = (double)edge[i][j];
        }
    }
    for (int k = 0; k < ng; k++){
        for (int i = 0; i < ng; i++){
            if (distance[i][k] < NONE_VALUE){
                for (int j = 0; j < ng; j++){
                    if (distance[i][k] + distance[k][j] < distance[i][j]){
                        distance[i][j] = distance[i][k] + distance[k][j];
                    }
                }
            }
        }
    }
}

double optimal_point(int point){
    double cur = 0, this_edge;
    for (int i = 0; i < ng; i++) {
        if (distance[point][i] > cur) {
            cur = distance[point][i];
        }
    }
    for (int i = 0; i < ng; i++){
        for (int j = i + 1; j < ng; j++){
            if (edge[i][j] < NONE_VALUE){
                if ((distance[point][i] < (distance[point][j] + (double)edge[i][j])) && (distance[point][j] < (distance[point][i] + (double)edge[i][j]))){
                    if (distance[point][i] < distance[point][j]){
                        this_edge = distance[point][j] + ((double)edge[i][j] - (distance[point][j] - distance[point][i]))/2;
                    } else{
                        this_edge = distance[point][i] + ((double)edge[i][j] - (distance[point][i] - distance[point][j]))/2;
                    }
                    if (this_edge > cur){
                        cur = this_edge;
                    }
                }
            }
        }
    }
    return cur;
}

void solve(){
    double cur;
    res = NONE_VALUE;
    for (int i = 0; i < ng; i++){
        if ((vertex[i].x % 2 == 0) && (vertex[i].y % 2 == 0)) {
            cur = optimal_point(i);
            if (cur < res) {
                res = cur;
                res_x = vertex[i].x / 2;
                res_y = vertex[i].y / 2;
            }
        }
    }
}
