////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>
using namespace std;

//////////////
// TYPEDEFS //
//////////////

struct VertexType
{
	float x;
	float y;
	float z;
};

struct FaceType
{
	int vIndex1;
	int vIndex2;
	int vIndex3;

	int tIndex1;
	int tIndex2;
	int tIndex3;

	int nIndex1;
	int nIndex2;
	int nIndex3;
};

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
void GetModelFilename(char* filename);
bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount);
bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount);

//////////////////
// MAIN PROGRAM //
//////////////////
int main()
{
	bool result;
	char filename[256];
	int vertexCount;
	int textureCount;
	int normalCount;
	int faceCount;

	//Read in the name of the model file
	GetModelFilename(filename);

	//Read in the number of vertices's, textures coordinates, normals, and faces so that the data structures can be initialized with the exact sizes needed.
	result = ReadFileCounts(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	//Display the counts to the screen for information purpose
	cout << "Vertices: " << vertexCount << endl;
	cout << "UVs: " << textureCount << endl;
	cout << "Normals: " << normalCount << endl;
	cout << "Faces: " << faceCount << endl;

	//Now read the data from the file into the data structures and then output it in our model format
	result = LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);
	if (!result)
	{
		return -1;
	}

	//Notify the user the model has been converted
	system("pause");
	return 0;
}

void GetModelFilename(char* filename)
{
	bool done;
	ifstream fIn;

	//Loop until we have a file name
	done = false;
	while (!done)
	{
		//Ask the user for the filename
		cout << "Enter model filename: ";

		//Read in the filename
		cin >> filename;

		//Attempt to open the file
		fIn.open(filename);

		if (fIn.good())
		{
			//If the file exist and there are no problems then exit since we have the file name
			fIn.close();
			done = true;
		}
		else
		{
			//If the file does not exist or there was an issue opening it then notify the user and repeat the process
			fIn.clear();
			cout << "\nFile " << filename << " could not be opened.\n\n";
		}
	}
	system("cls");
}

bool ReadFileCounts(char* filename, int& vertexCount, int& textureCount, int& normalCount, int& faceCount)
{
	ifstream fIn;
	char input;

	//Initialize the counts
	vertexCount = 0;
	textureCount = 0;
	normalCount = 0;
	faceCount = 0;

	//Open the file
	fIn.open(filename);

	//Check if it was successful in opening the file
	if (fIn.fail())
	{
		return false;
	}

	//Read from the file and continue to read until the end of the file is reached
	fIn.get(input);
	while (!fIn.eof())
	{
		//If the line starts with 'v' then count either the vertex, the texture coordinate or the normal vector
		if (input == 'v')
		{
			fIn.get(input);
			if (input == ' ')
			{
				vertexCount++;
			}
			else if (input == 't')
			{
				textureCount++;
			}
			else if (input == 'n')
			{
				normalCount++;
			}
		}
		//If the line starts with 'f' then increment the face counter
		else if (input == 'f')
		{
			fIn.get(input);
			if (input == ' ')
			{
				faceCount++;
			}
		}
		//Otherwise read in the remainder of the line
		while (input != '\n')
		{
			fIn.get(input);
		}
		//Start reading the beginning of the next line
		fIn.get(input);
	}
	//Close the file
	fIn.close();

	return true;
}

bool LoadDataStructures(char* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	VertexType* vertices = nullptr;
	VertexType* texcoords = nullptr;
	VertexType* normals = nullptr;
	FaceType* faces = nullptr;

	ofstream fOut;
	ifstream fIn;

	unsigned int vertexIndex = 0;
	unsigned int texcoordIndex = 0;
	unsigned int normalIndex = 0;
	unsigned int faceIndex = 0;
	int vIndex;
	int tIndex;
	int nIndex;

	char input;
	char input2;

	//Initialize the four data structures
	vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new VertexType[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new VertexType[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}

	//Open the file
	fIn.open(filename);
	if (fIn.fail())
	{
		return false;
	}

	//Read in the vertices, texture coordinates and normals into the data structures.
	//Important: Also convert to left hand coordinates system since Maya uses right hand coordinates system.
	fIn.get(input);
	while (!fIn.eof())
	{
		if (input == 'v')
		{
			fIn.get(input);

			//Read in the vertices
			if (input == ' ')
			{
				fIn >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				//Invert the Z vertex to change to left hand system
				vertices[vertexIndex].z *= -1.0f;
				vertexIndex++;
			}

			//Read in the texture uv coordinates
			else if (input == 't')
			{
				fIn >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				//Invert the V texture coordinates to left hand system
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			//Read in the normals
			else if (input == 'n')
			{
				fIn >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				//Invert the Z normal to change to left hand system
				normals[normalIndex].z *= -1.0f;
				normalIndex++;
			}
		}
		//Read in the faces
		else if (input == 'f')
		{
			fIn.get(input);
			if (input == ' ')
			{
				//Read the faces data in backwards to convert it to a left hand system from right hand system
				fIn >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		//Read in the remainder of the line
		while (input != '\n')
		{
			fIn.get(input);
		}

		//Start reading the beginning of the next line
		fIn.get(input);
	}

	//Close the file
	fIn.close();

	//Open the output file
	fOut.open("model.txt");

	//Write out the file header that our model format uses
	fOut << "Vertex Count: " << (faceCount * 3) << "\n\nData:\n\n";

	//Now loop through all the faces and output the three vertices for each face
	for (unsigned long i = 0; i < faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		fOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		fOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		fOut << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
			<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
			<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;
	}

	//Close the output file
	fOut.close();

	//Release the four data structures
	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}

	if (texcoords)
	{
		delete[] texcoords;
		texcoords = nullptr;
	}

	if (normals)
	{
		delete[] normals;
		normals = nullptr;
	}

	if (faces)
	{
		delete[] faces;
		faces = nullptr;
	}

	return true;
}