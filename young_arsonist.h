#define MAX_N 40

void load();
void build_graph();
void find_shortest_paths(); //алгоритм Флойда
void solve();
void save();

int n;  //количество исходных точек
int res_x, res_y; //координаты точки поджога
long double res; //минимальное время сгорания
struct {
    int x1, x2, y1, y2;
    long int time;

} match[MAX_N];    //исходные значения