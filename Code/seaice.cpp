#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "LinkedList.h"
#include "ProgressBar.h"
#include "ArrayList.h"

/*
[	63	 ]
.
.
.
[	63	 ]

Layers of those ^^
*/

#define MAX_WIDTH 6
#define N_YEARS_OF_DATA 16
#define GUI_FEATURES_ENABLE 1
#define EMPTY 157
#define LAND 168
float R_THRESH;
#define INF 99999

ArrayList<float> R;

using namespace std;

float year_data_array[16 * 52][MAX_WIDTH][MAX_WIDTH];
float time_series_mean[MAX_WIDTH][MAX_WIDTH];

// globals not good but cant help for now.

ArrayList<LinkedList> adjecency_list;
ArrayList<LinkedList> adjecency_list_random_graph;
ProgressBar progressBar(MAX_WIDTH * MAX_WIDTH, 70, '#', '-');

int convert_coordinate_into_linear_index(int x, int y) {
    return (x * MAX_WIDTH + y);
}
ArrayList<int> convert_linear_index_into_coordinate(int n) {
    ArrayList<int> v;

    v.push_back(n / MAX_WIDTH);
    v.push_back(n % MAX_WIDTH);

    return v;
}

void add_edge_to_graph_random(int x, int y, int x_, int y_) {
    int linear_index = convert_coordinate_into_linear_index(x, y);
    int linear_index_for_list = convert_coordinate_into_linear_index(x_, y_);

    adjecency_list_random_graph[linear_index].add(linear_index_for_list);
    adjecency_list_random_graph[linear_index_for_list].add(linear_index);
}

void add_edge_to_graph(int x, int y, int x_, int y_) {
    int linear_index = convert_coordinate_into_linear_index(x, y);
    int linear_index_for_list = convert_coordinate_into_linear_index(x_, y_);
     // cout << "linear_index = " << linear_index << " ; linear_index_for_list = " << linear_index_for_list << endl;
    adjecency_list[linear_index].add(linear_index_for_list);
    adjecency_list[linear_index_for_list].add(linear_index);
}
void load_year_into_array()
{
    int WK = 1;
    int YR = 1990;
    int index = 0;
    for (int i = 0; i < N_YEARS_OF_DATA; ++i)
    {
        string year = "1990";
        int WK = 1;
        for (int j = 0; j < 52; ++j)
        {
            string week = "01";
            if (WK <= 9)
            {
                std::string out_string;
                std::stringstream ss;
                ss << WK;
                out_string = ss.str();

                week = "0" + out_string;
            }
            else
            {
                std::string out_string;
                std::stringstream ss;
                ss << WK;
                out_string = ss.str();
                week = out_string;
            }

            std::string out_string;
            std::stringstream ss;
            ss << YR;
            year = ss.str();


            // cout << week << " " << year  << " " << index << endl;
            ifstream inputFile( "./CS310_project_subregion/" + year + "/Beaufort_Sea_diffw"+ week + "y"+ year + "+landmask", ios::in | ios::binary );
            inputFile.read( reinterpret_cast<char*> (year_data_array[index]), 4 * MAX_WIDTH*MAX_WIDTH ); // Read the entire damn file
            WK++;
            index++;
        }
        YR++;
    }
}
float calculate_Sxx(int x, int y)
{
    int layer_hieght = N_YEARS_OF_DATA*52;
    float X_bar = 0;
    float Sxx = 0;

    X_bar = time_series_mean[x][y];

    for (int h = 0; h < layer_hieght; ++h)
    {
        Sxx += ((year_data_array[h][x][y] - X_bar)*(year_data_array[h][x][y] - X_bar));
    }
    return Sxx;
}

float calculate_Syy(int x_, int y_)
{
    int layer_hieght = N_YEARS_OF_DATA*52;
    float Y_bar = 0;
    float Syy = 0;

    Y_bar = time_series_mean[x_][y_];

    for (int h = 0; h < layer_hieght; ++h)
    {
        Syy += ((year_data_array[h][x_][y_] - Y_bar)*(year_data_array[h][x_][y_] - Y_bar));
    }
    return Syy;
}

float calculate_Sxy(int x, int y, int x_, int y_)
{
    int layer_hieght = 16*52;
    float Sxy = 0;
    float Syy = 0;
    float X_bar = 0;
    float Y_bar = 0;

    X_bar = time_series_mean[x][y];
    Y_bar = time_series_mean[x_][y_];
    for (int h = 0; h < layer_hieght; ++h)
    {
        Sxy += ((year_data_array[h][x][y] - X_bar)*(year_data_array[h][x_][y_] - Y_bar));
    }
    return Sxy;
}

float calculate_r_coefficient(int x, int y, int x_, int y_)
{
    return (calculate_Sxy(x,y,x_,y_) / sqrt((calculate_Sxx(x,y) * calculate_Syy(x_,y_))));
}

void print_year_array() {
    for (int layer = 0; layer < 1; ++layer) {
        for (int i = 0; i < MAX_WIDTH; i++) {
            for (int j = 0; j < MAX_WIDTH; j++) {
                cout << year_data_array[layer][i][j] << ",";
            }
            cout << endl;
        }
    }
}

void print_mean_array() {
    for (int i = 0; i < MAX_WIDTH; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            cout << time_series_mean[i][j] << ",";
        }
        cout << endl;
    }

}

int *move_pointer_forward(int x, int y)
{
    int *coordinates = (int *) malloc(2 * sizeof(int));

    if (y == MAX_WIDTH - 1)
    {
        coordinates[0] = x + 1;
        coordinates[1] = 0;
    }
    else if (x == (MAX_WIDTH - 1) && y == (MAX_WIDTH - 1))
    {
        coordinates[0] = -1;
        coordinates[1] = -1;
    }
    else
    {
        coordinates[0] = x;
        coordinates[1] = y + 1;
    }
    // cout << coordinates[0] << "," << coordinates[1] << endl;
    return coordinates;
}

int *move_pointer_forward_floyd_warshall(int x, int y) {
    int *coordinates_fw = (int *) malloc(2 * sizeof(int));

    if (y == MAX_WIDTH - 1)
    {
        coordinates_fw[0] = x + 1;
        coordinates_fw[1] = 0;
    }
    else if (x == (MAX_WIDTH*MAX_WIDTH - 1) && y == (MAX_WIDTH*MAX_WIDTH - 1))
    {
        coordinates_fw[0] = -1;
        coordinates_fw[1] = -1;
    }
    else
    {
        coordinates_fw[0] = x;
        coordinates_fw[1] = y + 1;
    }
    return coordinates_fw;
}

void get_all_pairs_for_x_y(int x, int y) {
    int *next_pointer_coordinates = move_pointer_forward(x, y);

    while (next_pointer_coordinates[0] < (MAX_WIDTH)) {
        if (!(year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == LAND ||
              year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == EMPTY)) {
            // get only when it;s not a land / empty
            float r = calculate_r_coefficient(x, y, next_pointer_coordinates[0], next_pointer_coordinates[1]);
             // cout << "(" << next_pointer_coordinates[0] << "," << next_pointer_coordinates[1] << ")" << " " << r;

            if (r > R_THRESH) {
                // cout << "adding edge ... " <<  endl;
                add_edge_to_graph(x, y, next_pointer_coordinates[0], next_pointer_coordinates[1]);
            }
        }
        next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
    }
}

void make_the_graph() {
    int x = 0;
    int y = 0;

    int *next_pointer_coordinates = (int *) malloc(2 * sizeof(int));
    next_pointer_coordinates[0] = x;
    next_pointer_coordinates[1] = y;

    while (next_pointer_coordinates[0] < (MAX_WIDTH)) {
        if (!(year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == LAND ||
              year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == EMPTY)) {
            // get only when it;s not a land / empty
            get_all_pairs_for_x_y(next_pointer_coordinates[0], next_pointer_coordinates[1]);
        }
        if (GUI_FEATURES_ENABLE) {
            float progress = (float) convert_coordinate_into_linear_index(next_pointer_coordinates[0],
                                                                          next_pointer_coordinates[1]) /
                             (MAX_WIDTH * MAX_WIDTH);
            ++progressBar;
            progressBar.display();
        }
        next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
    }

}

void calculate_mean_array()
{
    for (int r = 0; r < MAX_WIDTH; ++r)
    {
        for (int c = 0; c < MAX_WIDTH; ++c)
        {
            float sum = 0;
            for (int h = 0; h < N_YEARS_OF_DATA*52; ++h)
            {
                sum += year_data_array[h][r][c];
            }
            time_series_mean[r][c] = sum / (N_YEARS_OF_DATA*52);
        }
    }
}

const int V = MAX_WIDTH*MAX_WIDTH;
bool visited[V];
ArrayList<int> component;

bool is_land(int li)
{
    ArrayList<int> c;
    c = convert_linear_index_into_coordinate(li);
    if ((year_data_array[0][c[0]][c[1]] == LAND ))
    {
        return true;
    }
    return false;
}
void visit(int v)
{
    // Mark the current node as visited and print it
    visited[v] = true;
    component.push_back(v);

    // Recur to visit all the vertices
    // adjacent to this vertex
    for(int i = 0; i < adjecency_list[v].length; ++i)
    {
        if((!visited[adjecency_list[v].at(i)]))
            if (!is_land(adjecency_list[v].at(i)))
            {
                visit(adjecency_list[v].at(i));
            }
    }
}
void connected_components() {
    int c = 1;
    // Mark all the vertices as not visited 
    for (int v = 0; v < V; v++)
        visited[v] = false;

    cout << "Component Analysis ... " << endl;
    for (int v = 0; v < V; v++) {
        if (!visited[v]) {
            component.clear();
            visit(v);
            cout << "Component " << c << " : " << component.size();
            cout << "\n";
            c++;
        }
    }
    cout << "===============================" << endl;
}

void get_all_pairs_for_x_y_random(int x, int y) {
    int *next_pointer_coordinates = move_pointer_forward(x, y);

    while (next_pointer_coordinates[0] < (MAX_WIDTH)) {
        if (rand() % 2) {
            add_edge_to_graph_random(x, y, next_pointer_coordinates[0], next_pointer_coordinates[1]);
        }
        next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
    }
}
void make_random_graph() {
    int x = 0;
    int y = 0;

    int *next_pointer_coordinates = (int *) malloc(2 * sizeof(int));
    next_pointer_coordinates[0] = x;
    next_pointer_coordinates[1] = y;

    while (next_pointer_coordinates[0] < (MAX_WIDTH)) {
        get_all_pairs_for_x_y_random(next_pointer_coordinates[0], next_pointer_coordinates[1]);
        next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
    }

}

// Floyd Marshall;
int dist[V][V];
int graph_f_m[V][V];
void printSolution() {
    cout << "The following matrix shows the shortest distances"
            " between every pair of vertices \n";
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (dist[i][j] == INF)
                cout << "-" << " ";
            else
                cout << dist[i][j] << " ";
        }
        cout << endl;
    }
}
int sum_of_pairs_with(int x, int y) {
    int sum = 0;
    int *next_pointer_coordinates = move_pointer_forward_floyd_warshall(x, y);

    while (next_pointer_coordinates[0] < V) {
        if ((dist[x][y] != INF) && (dist[next_pointer_coordinates[0]][next_pointer_coordinates[1]] != INF)) {
            sum += (dist[x][y] + dist[next_pointer_coordinates[0]][next_pointer_coordinates[1]]);

            // cout << sum << endl;
        }
        next_pointer_coordinates = move_pointer_forward_floyd_warshall(next_pointer_coordinates[0],
                                                                       next_pointer_coordinates[1]);
    }
    return sum;
}
void floydWarshall() {
    /* dist[][] will be the output matrix
    that will finally have the shortest
    distances between every pair of vertices */
    int i, j, k;

    /* Initialize the solution matrix same
    as input graph matrix. Or we can say
    the initial values of shortest distances
    are based on shortest paths considering
    no intermediate vertex. */
    for (i = 0; i < V; i++)
        for (j = 0; j < V; j++)
            dist[i][j] = graph_f_m[i][j];

    /* Add all vertices one by one to
    the set of intermediate vertices.
    ---> Before start of an iteration,
    we have shortest distances between all
    pairs of vertices such that the
    shortest distances consider only the
    vertices in set {0, 1, 2, .. k-1} as
    intermediate vertices.
    ----> After the end of an iteration,
    vertex no. k is added to the set of
    intermediate vertices and the set becomes {0, 1, 2, .. k} */
    for (k = 0; k < V; k++) {
        // Pick all vertices as source one by one
        for (i = 0; i < V; i++) {
            // Pick all vertices as destination for the
            // above picked source
            for (j = 0; j < V; j++) {
                // If vertex k is on the shortest path from
                // i to j, then update the value of dist[i][j]
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    // Print the shortest distance matrix  
    printSolution();  
    // int x = 0;
    // int y = 0;
    // int sum = 0;
    // int *next_pointer_coordinates = (int *) malloc(2 * sizeof(int));
    // next_pointer_coordinates[0] = x;
    // next_pointer_coordinates[1] = y;

    // while (next_pointer_coordinates[0] < (MAX_WIDTH * MAX_WIDTH)) {
    //     sum += sum_of_pairs_with(next_pointer_coordinates[0], next_pointer_coordinates[1]);
    //     next_pointer_coordinates = move_pointer_forward_floyd_warshall(next_pointer_coordinates[0],
    //                                                                    next_pointer_coordinates[1]);
    // }

    // cout << "Characteristic Path length = " << (float) sum / (MAX_WIDTH * MAX_WIDTH * MAX_WIDTH * MAX_WIDTH) << endl;
}
// ===============

int main(int argc, char const *argv[]) {
//    What R values you want?
    R.push_back(0.95);
//    R.push_back(0.925);
//    R.push_back(0.9);

    // for probability of edge calculations
    double k = 0.0;
    int n = MAX_WIDTH * MAX_WIDTH;
    load_year_into_array();
    calculate_mean_array();

    // floyd warshall algorithm prep.
    for (int i = 0; i < MAX_WIDTH * MAX_WIDTH; ++i) {
        for (int j = 0; j < MAX_WIDTH * MAX_WIDTH; ++j) {
            if (i == j) {
                graph_f_m[i][j] = 0;
            } else {
                graph_f_m[i][j] = INF;
            }
        }
    }
    cout << "The program does not exclude the LAND components in component analysis" << endl;
    for (int r_value = 0; r_value < R.size(); ++r_value)
    {
        R_THRESH = R[r_value];

        ArrayList<LinkedList> adjecency_list_local;

        for (int i = 0; i < MAX_WIDTH*MAX_WIDTH; ++i)
        {
            adjecency_list_local.push_back(LinkedList());
        }

        adjecency_list = adjecency_list_local;


        make_the_graph();

        ArrayList<int> degree_distribution;
        ArrayList<int> list_degree_data;

        for (int i = 0; i < adjecency_list.size(); ++i)
        {
            list_degree_data.push_back(adjecency_list[i].length);
            if (!(degree_distribution.has(adjecency_list[i].length)))
            {
                degree_distribution.push_back(adjecency_list[i].length);
            }
        }

        for (int i = 0; i < MAX_WIDTH*MAX_WIDTH; ++i)
        {
        	for (int j = 0; j < MAX_WIDTH*MAX_WIDTH; ++j)
        	{
        		
        	}
        }
        degree_distribution.sort();

        cout << endl;
        cout << "\n =========================== Histogram for R_THRESH = "  << R[r_value] << " << ===========================  \n";
        for (int i = 1; i < degree_distribution.size(); ++i)
        {
            cout << degree_distribution[i] << "\t";
            for (int j = 0; j < list_degree_data.frequency(degree_distribution[i]); j = j + 5)
            {
                cout << "* ";
            }
            cout << " " <<  list_degree_data.frequency(degree_distribution[i]);
            cout << endl;
        }

        cout << "One * = 5 frequency" << endl;
        progressBar.reset();
        cout << endl;

//        connected_components();
        floydWarshall();

    }

    return 0;
}
