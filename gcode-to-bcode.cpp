#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

struct bcode {
	unsigned char x_whole;
	unsigned char x_dec;
	unsigned char y_whole;
	unsigned char y_dec;
};

int main( int argc, char** argv )
{
	if (argc != 3)
	{
	cerr << "usage:\n"
		"  gcode-to-bcode infile outfile\n\n";
	return 1;
	}

	struct stat filestatus;
	stat( argv[1], &filestatus );

	fstream input;
	input.open(argv[1], fstream::in);
	fstream output;
	output.open(argv[2], fstream::out | fstream::binary);

	string line;

	cout << "First 20 lines:" << endl;
	for (int i = 0; i < 20 && getline (input,line); ++i )
	{
		cout << line << endl;
	}
	cout << "Which coordinates to use? (XYZ): ";
	string coords;
	cin >> coords;
	cout << endl << endl;
	
	input.seekg(0, std::ios::beg);
	while ( getline (input,line) )
	{
		char * command = strtok((char *)line.c_str(), " ");
		if(command != NULL)
		{
			if(strcmp(command, "G01") && strcmp(command, "G1") && strcmp(command, "G00") && strcmp(command, "G0") )
			{
				cout << "BAD: `" << command <<"`" << endl;
				continue;
			}
			else
			{
				command = strtok(NULL, " ");	
				bcode move;
				do
				{	
					
					if(coords.find(command[0]) != string::npos)
					{
						if(command[0] == 'X')
						{
							float x = stof(string(command).substr(1));
							move.x_whole = (int) x % 10000; // just a large number
							move.x_dec = (int) (x - move.x_whole) << 8;
						}
						else
						{
							float y = stof(string(command).substr(1));
							move.y_whole = (int) y % 10000; // just a large number
							move.y_dec = (int)(y - move.x_whole) << 8;
						}
					}

					command = strtok(NULL, " ");
				}while(command != NULL);
				output << move.x_whole << move.x_dec << move.y_whole<< move.y_dec;
			}
		}
	}

	output.close();
	input.close();
	float orig = filestatus.st_size;
	cout << "Original size: " << filestatus.st_size << " bytes\n";
	 
	stat( argv[2], &filestatus );
	cout << "New size: " << filestatus.st_size << " bytes\n";
	cout << (float) filestatus.st_size / orig * 100 << "% commpression ratio" << endl;
	return 0;
}

