#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>

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
#define R_THRESH 1.0

using namespace std;

float year_data_array[16*52][63][63];

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

int main(int argc, char const *argv[])
{	
	load_year_into_array();
	// print_year_array();


	cout << calculate_r_coefficient(26,59,30,21) << endl;

	system("pause");
	return 0;
}