#include<gl/GL.h>
#include<stdio.h>
#include<vector>
#include<stdlib.h>

#define NR_POINT_COORDS			3	//Number of point coordinates
#define NR_TEXTURE_COORDS		2	//Number of texture coordinates
#define NR_NORMAL_COORDS		3	//Number of normal coordinates
#define NR_FACE_TOKENS			3	//Minimum number of entries in face data

#define BUFFER_SIZE				256
#define S_EQUAL					0

FILE *g_fp_meshfile = NULL;
FILE *gpFile;
char gszLogFileName[] = "Log.txt";
//Vector of vector of floats to hold vertex data
std::vector<std::vector<float>> g_temp_vertices;

//Vector of vector of floats to hold texture data
std::vector<std::vector<float>> g_temp_textures;

//Vector of vector of floats to hold normal data
std::vector<std::vector<float>> g_temp_normals;

//Vector of vector of int to hold index data in g_temp_vertices
std::vector<std::vector<int>> g_face_tri, g_face_texture, g_face_normals;

int count_of_vertices = 0;

char line[BUFFER_SIZE];

TCHAR str[256];

int LoadMeshData(char *filename, std::vector<float> &vertices, std::vector<float> &textures, std::vector<float> &normals)
{
	//Open Mesh file, name of mesh file can be parameterized
	//g_fp_meshfile = fopen(filename, "r");
	fopen_s(&g_fp_meshfile, filename, "r");


	if (!g_fp_meshfile)
		exit(EXIT_FAILURE);

	//Separator strings
	//String holding space separator for strtok
	char *sep_space = " ";
	//String holding forward slash separator for strtok
	char *sep_fslash = "/";

	//Token pointers
	//Character pointer for holding first word in a line
	char *first_token = NULL;
	//Character pointer for holding next word separated by
	//Specified separator to strtok
	char *token = NULL;

	//Array of character pointers to hold strings of face entries
	//Face entries can be variable. In some files they are three and in some files they are four.
	char *face_tokens[NR_FACE_TOKENS];
	//Number of non-null tokens in the above vector
	int nr_tokens;

	//Character pointer holding string associated with
	//vertex index
	char *token_vertex_index = NULL;
	//Character pointer holding string associated with
	//texture index
	char *token_texture_index = NULL;
	//Character pointer holding string associated with
	//normal index
	char *token_normal_index = NULL;

	//While there is a line in file
	while (fgets(line, BUFFER_SIZE, g_fp_meshfile) != NULL)
	{
		/*fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf(gpFile, "In LoadMeshData While Loop\n");
		fclose(gpFile);*/
		//Bind line to a separator and get first token
		//first_token = strtok(line, sep_space);
		char *next = NULL;
		first_token = strtok_s(line, sep_space, &next);
		
		/*fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf(gpFile, "In LoadMeshData After strtok_s While Loop\n");
		fclose(gpFile);*/
		//If first token indicates vertex data
		if (strcmp(first_token, "v") == S_EQUAL)
		{
			/*fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf(gpFile, "In LoadMeshData While Loop v Token\n");
			fclose(gpFile);*/
			/*Create a vector of NR_POINT_COORDS number of floats
			to hold coordinates*/
			std::vector<float>vec_point_coord(NR_POINT_COORDS);

			//Do following NR_POINTS_COORDS time
			//S1. Get next token
			//S2. Feed it to atof to get floating point number out of it
			//S3. And the floating point number generated to vector
			//End of loop
			/*S4. At the end of loop vector is constructed, add it to
			global vector of vector of floats, named g_temp_vertices*/
			char *temp = NULL;
			for (int i = 0; i != NR_POINT_COORDS; i++)
			{
				//vec_point_coord[i] = atof(strtok(NULL, sep_space));	//S1, S2, S3
																	//vertices_arr[gvertices_count] = vec_point_coord[i];
																	//vertices_arr[gvertices_count] = atof(strtok(NULL, sep_space));
																	//gvertices_count++;
				
				//strtok_s(temp, NULL, &sep_space);
				vec_point_coord[i]  = atof(strtok_s(NULL, sep_space, &next));
				//vec_point_coord[i] = atof(temp);
				
			}
			g_temp_vertices.push_back(vec_point_coord);
		}

		//If first token indicates texture data
		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			/*fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf(gpFile, "In LoadMeshData While Loop vt Token\n");
			fclose(gpFile);*/

			/*Create a vector of NR_TEXTURE_COORDS number of floats
			to hold coordinates*/
			std::vector<float>vec_texture_coord(NR_TEXTURE_COORDS);

			//Do following NR_TEXTURE_COORDS time
			//S1. Get next token
			//S2. Feed it to atof to get floating point number out of it
			//S3. And the floating point number generated to vector
			//End of loop
			/*S4. At the end of loop vector is constructed, add it to
			global vector of vector of floats, named g_temp_textures*/
			char *temp = NULL;
			for (int i = 0; i != NR_TEXTURE_COORDS; i++)
			{
				//vec_texture_coord[i] = atof(strtok(NULL, sep_space));
				//strtok_s(temp, NULL, &sep_space);
				vec_texture_coord[i] = atof(strtok_s(NULL, sep_space, &next));
				//vec_texture_coord[i] = atof(temp);
			}	//S1, S2, S3
			g_temp_textures.push_back(vec_texture_coord);
		}

		//If first token indicates normal data
		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			/*fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf(gpFile, "In LoadMeshData While Loop vn Token\n");
			fclose(gpFile);*/
			/*Create a vector of NR_NORMAL_COORDS number of floats
			to hold coordinates*/
			std::vector<float>vec_normal_coord(NR_NORMAL_COORDS);

			//Do following NR_NORMAL_COORDS time
			//S1. Get next token
			//S2. Feed it to atof to get floating point number out of it
			//S3. And the floating point number generated to vector
			//End of loop
			/*S4. At the end of loop vector is constructed, add it to
			global vector of vector of floats, named g_normal*/
			char *temp = NULL;
			for (int i = 0; i != NR_NORMAL_COORDS; i++)
			{
				//vec_normal_coord[i] = atof(strtok(NULL, sep_space));	//S1, S2, S3
				//strtok_s(temp, NULL, &sep_space);
				vec_normal_coord[i] = atof(strtok_s(NULL, sep_space, &next));
				//vec_normal_coord[i] = atof(temp);
			}
			g_temp_normals.push_back(vec_normal_coord);
		}

		//If first token indicates face data
		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			
			/*Define three vector of integers with length 3 to hold indices of
			triangle's positional coordinates, texture coordinates, and normal
			coordinates in g_temp_vertices,g_temp_texturess and g_temp_normals resp*/
			std::vector<int> triangle_vertex_indices(3), texture_vertex_indices(3), normal_vertex_indices(3);

			//Initialize all char pointers in face_tokens to NULL
			memset((void*)face_tokens, 0, NR_FACE_TOKENS);

			//Extract three fields of information in face_tokens
			//and increment nr_tokens accordingly
			nr_tokens = 0;
		//	while (token = strtok(NULL, sep_space))
		//	while(strtok_s(token, NULL, &sep_space))
			while(token = strtok_s(NULL, sep_space, &next))
			{
				if (strlen(token) < 3)
					break;
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i != NR_FACE_TOKENS; ++i)
			{
			
				token_vertex_index = strtok_s(face_tokens[i], sep_fslash, &next);
				token_texture_index = strtok_s(NULL, sep_fslash, &next);
				token_normal_index = strtok_s(NULL, sep_fslash, &next);
				triangle_vertex_indices[i] = atoi(token_vertex_index);
				texture_vertex_indices[i] = atoi(token_texture_index);
				normal_vertex_indices[i] = atoi(token_normal_index);
				//face_arr[count_face]= (GLsizei)atoi(token_vertex_index);
				//count_face++;
			}

			//Add constructed vectors to global face vectors
			g_face_tri.push_back(triangle_vertex_indices);
			g_face_texture.push_back(texture_vertex_indices);
			g_face_normals.push_back(normal_vertex_indices);
		}
		else
		{
			
		}
	
		//Initialize line buffer to NULL
		memset((void*)line, (int)'\0', BUFFER_SIZE);
	}

	//Close meshfile and make file pointer NULL
	fprintf(g_fp_meshfile, "1\n");
	fclose(g_fp_meshfile);
	g_fp_meshfile = NULL;

	for (int i = 0; i != g_face_tri.size(); ++i)
	{
		for (int j = 0; j != g_face_tri[i].size(); j++)
		{
			int vi = g_face_tri[i][j] - 1;
			int vn = g_face_normals[i][j] - 1;
			int vt = g_face_texture[i][j] - 1;
			//int vt = g_face_texture[i][j] - 1;
			vertices.push_back( g_temp_vertices[vi][0] );
			vertices.push_back(g_temp_vertices[vi][1]);
			vertices.push_back(g_temp_vertices[vi][2]);
			normals.push_back(g_temp_normals[vn][0]);
			normals.push_back(g_temp_normals[vn][1]);
			normals.push_back(g_temp_normals[vn][2]);
			textures.push_back(g_temp_textures[vt][0]);
			textures.push_back(g_temp_textures[vt][1]);
			count_of_vertices += 3;
		}
	}

	g_temp_vertices.clear();
	g_temp_textures.clear();
	g_temp_normals.clear();
	g_face_tri.clear();
	g_face_texture.clear();
	g_face_normals.clear();

	
	return(count_of_vertices);
}
