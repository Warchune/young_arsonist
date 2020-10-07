#include <stdio.h>

#define FILE_IN "./files/f.in"
#define FILE_OUT "./files/f.out"
#define MAX_N 40
#define MAX_NG (MAX_N * 2)
#define NONE_VALUE 10000001L

struct m_date{  //match_date
    int x1, x2, y1, y2;
    long int time;
};
struct arr_m_date{  //array_match_date
    int n;  //количество спичек
    struct m_date match[MAX_N];
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

struct arr_m_date read();
struct g_date make_graph(struct arr_m_date);
struct p_date calculate(struct g_date);
void write(struct p_date);


int main(){
    struct arr_m_date date;
    struct g_date graph;
    struct p_date point;

    date = read();

    graph = make_graph(date);
    point = calculate(graph);

    write(point);

    return 0;
}

struct arr_m_date read(){
    struct arr_m_date date;
    FILE *f_in;
    f_in = fopen(FILE_IN, "r");
    fscanf(f_in, "%d", &date.n);
    for (int i = 0; i < date.n; i++){
        fscanf(f_in, "%d%d%d%d%ld", &date.match[i].x1, &date.match[i].y1, &date.match[i].x2, &date.match[i].y2, &date.match[i].time);
    }
    fclose(f_in);

    return date;
}

int get_vertex(int vx, int vy, struct g_date *graph){
    for (int i = 0; i < (*graph).ng; i++){   //проверка на уже существующую вершину
        if(((*graph).vertex[i].x == vx) && ((*graph).vertex[i].y == vy)) {
            return i;
        }
    }
    //ng++;
    (*graph).vertex[(*graph).ng].x = vx;
    (*graph).vertex[(*graph).ng].y = vy;
    for (int i = 0; i != (*graph).ng; i++){  //заполнение несуществующих ребер
        (*graph).edge[i][(*graph).ng] = NONE_VALUE;
        (*graph).edge[(*graph).ng][i] = NONE_VALUE;
    }
    (*graph).edge[(*graph).ng][(*graph).ng] = 0;
    (*graph).ng++;
    return (*graph).ng - 1;

}

void add_edge(int x1, int y1, int x2,int  y2, long int time, struct g_date *graph){
    (*graph).edge[get_vertex(x1, y1, graph)][get_vertex(x2, y2, graph)] = time;
    (*graph).edge[get_vertex(x2, y2, graph)][get_vertex(x1, y1, graph)] = time;
}

struct g_date make_graph(struct arr_m_date date){
    struct g_date graph;
    graph.ng = 0;
    for (int i = 0; i < date.n; i++){
        //  умножение координат на 2, соответственно удвоение спичек
        printf("add_edge(%d, %d, %d, %d, %ld);\n",date.match[i].x1 * 2, date.match[i].y1 * 2, date.match[i].x1 + date.match[i].x2, date.match[i].y1 + date.match[i].y2, date.match[i].time);
        add_edge(date.match[i].x1 * 2, date.match[i].y1 * 2, date.match[i].x1 + date.match[i].x2, date.match[i].y1 + date.match[i].y2, date.match[i].time, &graph);
        printf("add_edge(%d, %d, %d, %d, %ld);\n", date.match[i].x1 + date.match[i].x2, date.match[i].y1 + date.match[i].y2, date.match[i].x2 * 2, date.match[i].y2 * 2, date.match[i].time);
        add_edge(date.match[i].x1 + date.match[i].x2, date.match[i].y1 + date.match[i].y2, date.match[i].x2 * 2, date.match[i].y2 * 2, date.match[i].time, &graph);
    }
    for (int i = 0; i < graph.ng; i++){
        for (int j = 0; j < graph.ng; j++){
            printf("%4ld ",graph.edge[i][j]);
        }
        printf("\n");
    }

    return graph;
}

double get_time_at(int point, struct g_date graph, double distance[MAX_NG][MAX_NG]){
    double cur = 0, this_edge;
    for (int i = 0; i < graph.ng; i++) {
        if (distance[point][i] > cur) {
            cur = distance[point][i];
        }
    }
    printf("%lf\n", cur);
    for (int i = 0; i < graph.ng; i++){
        for (int j = i + 1; j < graph.ng; j++){
            if (graph.edge[i][j] < NONE_VALUE){
                printf("edge[%d,%d](%ld) < 999\n", i, j, graph.edge[i][j]);
                if ((distance[point][i] < (distance[point][j] + (double)graph.edge[i][j])) && (distance[point][j] < (distance[point][i] + (double)graph.edge[i][j]))){
                    printf("(d[%d,%d](%lf) < (d[%d,%d](%lf) + e[%d,%d](%lf)) && (d[%d,%d](%lf) < (d[%d,%d](%lf) + e[%d,%d](%lf))\n", point, i, distance[point][i], point, j, distance[point][j], i, j, (double)graph.edge[i][j], point, j,distance[point][j] ,point, i,distance[point][i] , i, j, (double)graph.edge[i][j]);
                    if (distance[point][i] < distance[point][j]){
                        this_edge = distance[point][j] + ((double)graph.edge[i][j] - (distance[point][j] - distance[point][i]))/2;
                        printf("%lf = d[%d,%d](%lf) + (e[%d,%d](%lf) - (d[%d,%d](%lf) - d[%d,%d](%lf)))/2\n", this_edge, point, i, distance[point][j], i, j, (double)graph.edge[i][j], point, j, distance[point][j], point, i, distance[point][i] );
                    } else{
                        this_edge = distance[point][i] + ((double)graph.edge[i][j] - (distance[point][i] - distance[point][j]))/2;
                        printf("%lf = d[%d,%d](%lf) + (e[%d,%d](%lf) - (d[%d,%d](%lf) - d[%d,%d](%lf)))/2\n", this_edge, point, i, distance[point][j], i, j, (double)graph.edge[i][j], point, i, distance[point][j], point, j, distance[point][i] );
                    }
                    if (this_edge > cur){
                        cur = this_edge;
                        printf("current_time - %lf\n", cur);
                    }
                }
            }
        }
    }
    return cur;
}

struct p_date calculate(struct g_date graph){
    struct p_date point;
    double distance[MAX_NG][MAX_NG];
    double current_time;

    for (int i = 0; i < graph.ng; i++){
        for (int j = 0; j < graph.ng; j++){
            distance[i][j] = (double)graph.edge[i][j];
        }
    }
    //алгоритм Флойда
    for (int k = 0; k < graph.ng; k++){
        for (int i = 0; i < graph.ng; i++){
            if (distance[i][k] < NONE_VALUE){
                for (int j = 0; j < graph.ng; j++){
                    if (distance[i][k] + distance[k][j] < distance[i][j]){
                        distance[i][j] = distance[i][k] + distance[k][j];
                    }
                }
            }
        }
    }
    for (int i = 0; i < graph.ng; i++){
        for (int j = 0; j < graph.ng; j++){
            printf("%4lf ",distance[i][j]);
        }
        printf("\n");
    }

    point.time = NONE_VALUE;
    for (int i = 0; i < graph.ng; i++){
        if ((graph.vertex[i].x % 2 == 0) && (graph.vertex[i].y % 2 == 0)) {
            printf("%d(%d,%d) - ", i, graph.vertex[i].x, graph.vertex[i].y);
            current_time = get_time_at(i, graph, distance);
            if (current_time < point.time) {
                point.time = current_time;
                point.x = graph.vertex[i].x / 2;
                point.y = graph.vertex[i].y / 2;
            }
        }
    }

    return point;
}

void write(struct p_date point){
    FILE *f_out;
    f_out = fopen(FILE_OUT, "w");
    fprintf(f_out, "%d %d\n"
                   "%.2f\n", point.x, point.y, point.time/2);
    fclose(f_out);
}