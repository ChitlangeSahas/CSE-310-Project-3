#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include "LinkedList.h"
#include "ArrayList.h"

#define MAX_WIDTH 63
/*
[	63	 ]
.
.
.
[	63	 ]

Layers of those ^^
*/

#define EMPTY 157
#define LAND 168
#define R_THRESH 0.95

using namespace std;

float year_data_array[16*52][63][63];
ArrayList<LinkedList> adjecency_list;

int convert_coordinate_into_linear_index(int x, int y)
{
	return (x*MAX_WIDTH + y);
}

ArrayList<int> convert_linear_index_into_coordinate(int n)
{
	ArrayList<int> v;

	v.push_back(n / MAX_WIDTH);
	v.push_back(n % MAX_WIDTH);

	return v;
}

void add_edge_to_graph(int x, int y, int x_, int y_)
{
	int linear_index = convert_coordinate_into_linear_index(x,y);
	int linear_index_for_list = convert_coordinate_into_linear_index(x_, y_);
	// cout << "linear_index = " << linear_index << " ; linear_index_for_list = " << linear_index_for_list << endl;

	adjecency_list[linear_index].add(linear_index_for_list);

}

void load_year_into_array()
{
	int WK = 1;
	int YR = 1990;
	int index = 0;
	for (int i = 0; i < 16; ++i)
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
				inputFile.read( reinterpret_cast<char*> (year_data_array[index]), 4 * 3969 ); // Read the entire damn file	
				WK++;
			index++;
		}
	YR++;
	}

}

float calculate_Sxx(int x, int y)
{
	int layer_hieght = 16*52;
	float X_bar = 0;
	float Sxx = 0;

	for (int h = 0; h < layer_hieght; ++h)
	{
		X_bar += year_data_array[h][x][y];
	}
	X_bar = X_bar / (float) layer_hieght;
	
	for (int h = 0; h < layer_hieght; ++h)
	{
		Sxx += ((year_data_array[h][x][y] - X_bar)*(year_data_array[h][x][y] - X_bar));
	}
	return Sxx;	
}

float calculate_Syy(int x_, int y_)
{
	int layer_hieght = 16*52;
	float Y_bar = 0;
	float Syy = 0;

	for (int h = 0; h < layer_hieght; ++h)
	{
		Y_bar += year_data_array[h][x_][y_];
	}
	Y_bar = Y_bar / (float) layer_hieght;
  
	
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

	for (int h = 0; h < layer_hieght; ++h)
	{
		Y_bar += year_data_array[h][x_][y_];
	}
	Y_bar = Y_bar / (float) layer_hieght;

	for (int h = 0; h < layer_hieght; ++h)
	{
		X_bar += year_data_array[h][x][y];
	}
	X_bar = X_bar / (float) layer_hieght;
	
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

void print_year_array()
{
	for (int layer = 0; layer < 1; ++layer)
	{
		for (int i = 0; i < 63; i++) 
		{
        	for (int j = 0; j < 63; j++) 
        	{
            	cout << year_data_array[layer][i][j] << ",";
        	}
        cout << endl;
    	}
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
	return coordinates;
}


void get_all_pairs_for_x_y(int x, int y)
{
	int *next_pointer_coordinates = move_pointer_forward(x,y);
	
	while(next_pointer_coordinates[0] < (MAX_WIDTH))
	{
		if (!(year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == LAND || year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == EMPTY))
		{
			// get only when it;s not a land / empty
			float r = calculate_r_coefficient(x, y , next_pointer_coordinates[0] , next_pointer_coordinates[1]);
			// cout << "(" << next_pointer_coordinates[0] << "," << next_pointer_coordinates[1] << ")" << " " << r;

			if (r > R_THRESH)
			{
				add_edge_to_graph(x,y,next_pointer_coordinates[0], next_pointer_coordinates[1]);
			}
		}
		next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
	}
}

void make_the_graph()
{
	int x = 0;
	int y = 0;

	int *next_pointer_coordinates = (int *) malloc(2 * sizeof(int));
	next_pointer_coordinates[0] = x;
	next_pointer_coordinates[1] = y;

	while(next_pointer_coordinates[0] < (MAX_WIDTH))
	{
		// cout << "\nPairs for ";
		// cout << "(" << next_pointer_coordinates[0] << "," << next_pointer_coordinates[1] << ")" << endl;
		if (!(year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == LAND || year_data_array[0][next_pointer_coordinates[0]][next_pointer_coordinates[1]] == EMPTY))
		{
			// get only when it;s not a land / empty
			get_all_pairs_for_x_y(next_pointer_coordinates[0],next_pointer_coordinates[1]);
		}
		next_pointer_coordinates = move_pointer_forward(next_pointer_coordinates[0], next_pointer_coordinates[1]);
	}

}

int main(int argc, char const *argv[])
{	
	load_year_into_array();
	cout << "Hold Tight..." << endl; 

	// fill up the damn adjecency list with blank values.
	for (int i = 0; i < MAX_WIDTH*MAX_WIDTH; ++i)
	{
		adjecency_list.push_back(LinkedList());
	}
	
	make_the_graph();

	ArrayList<int> degree_distribution;

	for (int i = 0; i < adjecency_list.size(); ++i)
	{
		cout << i << " : " ; 
		adjecency_list[i].print();
		if (!(degree_distribution.has(adjecency_list[i].length)))
		{
			degree_distribution.push_back(adjecency_list[i].length);
		}
	}



	// system("pause");
	return 0;
}