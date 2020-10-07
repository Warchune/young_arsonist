#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define FILE_IN "./files/f.in"
#define FILE_OUT "./files/f.out"
#define MAX_L 40    //максимальное количество спичек
#define MAX_NG (MAX_L * 2)  //максильное количесвтво точек графа
#define NONE_VALUE 10000001L

struct match{  //спичка
    int x1, x2, y1, y2;
    long int time;
};
struct match_list{  //список спичек
    int len;  //количество спичек
    struct match list[MAX_L];
};
struct v_date{  //вершина графа
    int x, y;
};
struct g_date{  //graph_date
    int ng; //количество вершин в графе
    long int edge[MAX_NG][MAX_NG];  //ребра графа
    struct v_date vertex[MAX_NG];   //вершины графа
};
struct p_date{  //point_date - точка поджога
    int x, y;
    double time;
};

struct match_list read();
struct g_date make_graph(struct match_list);
struct p_date calculate(struct g_date);
void write(struct p_date);


int main(){
    struct match_list d;
    struct g_date g;
    struct p_date p;

    d = read();


    g = make_graph(d);
    p = calculate(g);

    write(p);

    return 0;
}

struct match_list read(){
    struct match_list d;
    FILE *f_in;
    f_in = fopen(FILE_IN, "r");
    if (f_in) {
        fscanf(f_in, "%d", &d.len);
        for (int i = 0; i < d.len; i++) {
            fscanf(f_in, "%d%d%d%d%ld", &d.list[i].x1, &d.list[i].y1, &d.list[i].x2, &d.list[i].y2,
                   &d.list[i].time);
        }
        fclose(f_in);
    }
    else{
        perror("read: fopen() ");
        exit(errno);
    }

    return d;
}

int get_vertex(int vx, int vy, struct g_date *g){
    for (int i = 0; i < (*g).ng; i++){   //проверка на уже существующую вершину
        if(((*g).vertex[i].x == vx) && ((*g).vertex[i].y == vy)) {
            return i;
        }
    }
    //ng++;
    (*g).vertex[(*g).ng].x = vx;
    (*g).vertex[(*g).ng].y = vy;
    for (int i = 0; i != (*g).ng; i++){  //заполнение несуществующих ребер
        (*g).edge[i][(*g).ng] = NONE_VALUE;
        (*g).edge[(*g).ng][i] = NONE_VALUE;
    }
    (*g).edge[(*g).ng][(*g).ng] = 0;
    (*g).ng++;
    return (*g).ng - 1;

}

void add_edge(int x1, int y1, int x2,int  y2, long int time, struct g_date *g){
    (*g).edge[get_vertex(x1, y1, g)][get_vertex(x2, y2, g)] = time;
    (*g).edge[get_vertex(x2, y2, g)][get_vertex(x1, y1, g)] = time;
}

struct g_date make_graph(struct match_list d){
    struct g_date g;
    g.ng = 0;
    for (int i = 0; i < d.len; i++){
        //  умножение координат на 2, соответственно удвоение спичек
        add_edge(d.list[i].x1 * 2, d.list[i].y1 * 2, d.list[i].x1 + d.list[i].x2, d.list[i].y1 + d.list[i].y2, d.list[i].time, &g);
        add_edge(d.list[i].x1 + d.list[i].x2, d.list[i].y1 + d.list[i].y2, d.list[i].x2 * 2, d.list[i].y2 * 2, d.list[i].time, &g);
    }

    return g;
}

double get_time_at(int p, struct g_date g, double distance[MAX_NG][MAX_NG]){
    double current_time = 0;
    double this_edge;
    for (int i = 0; i < g.ng; i++) {
        if (distance[p][i] > current_time) {
            current_time = distance[p][i];
        }
    }
    for (int i = 0; i < g.ng; i++){
        for (int j = i + 1; j < g.ng; j++){
            if (g.edge[i][j] < NONE_VALUE){
                if ((distance[p][i] < (distance[p][j] + (double)g.edge[i][j])) && (distance[p][j] < (distance[p][i] + (double)g.edge[i][j]))){
                    if (distance[p][i] < distance[p][j]){
                        this_edge = distance[p][j] + ((double)g.edge[i][j] - (distance[p][j] - distance[p][i])) / 2;
                    } else{
                        this_edge = distance[p][i] + ((double)g.edge[i][j] - (distance[p][i] - distance[p][j])) / 2;
                    }
                    if (this_edge > current_time){
                        current_time = this_edge;
                    }
                }
            }
        }
    }
    return current_time;
}

struct p_date calculate(struct g_date g){
    struct p_date p;
    double distance[MAX_NG][MAX_NG];
    double current_time;

    for (int i = 0; i < g.ng; i++){
        for (int j = 0; j < g.ng; j++){
            distance[i][j] = (double)g.edge[i][j];
        }
    }
    //алгоритм Флойда
    for (int k = 0; k < g.ng; k++){
        for (int i = 0; i < g.ng; i++){
            if (distance[i][k] < NONE_VALUE){
                for (int j = 0; j < g.ng; j++){
                    if (distance[i][k] + distance[k][j] < distance[i][j]){
                        distance[i][j] = distance[i][k] + distance[k][j];
                    }
                }
            }
        }
    }

    p.time = NONE_VALUE;
    for (int i = 0; i < g.ng; i++){
        if ((g.vertex[i].x % 2 == 0) && (g.vertex[i].y % 2 == 0)) {
            current_time = get_time_at(i, g, distance);
            if (current_time < p.time) {
                p.time = current_time;
                p.x = g.vertex[i].x / 2;
                p.y = g.vertex[i].y / 2;
            }
        }
    }

    return p;
}

void write(struct p_date p){
    FILE *f_out;
    f_out = fopen(FILE_OUT, "w");
    if (f_out) {
        fprintf(f_out, "%d %d\n"
                       "%.2f\n", p.x, p.y, p.time / 2);
        fclose(f_out);
    }
    else{
        perror("write: fopen() ");
        exit(errno);
    }
}