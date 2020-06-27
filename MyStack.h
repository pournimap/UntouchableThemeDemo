#pragma once
//#include "Common.h"
#include <windows.h>
#include <windowsx.h> //For GET_X_LPARAM and GET_Y_LPARAM

#include <stdio.h> // for FILE I/O

#include <gl\glew.h>//use it before other GL.h

#include <gl/GL.h>
#include "vmath.h"
using namespace vmath;

#include "vmath.h"
using namespace vmath;

struct Node
{
	//int data;//integer data
	mat4 modelMatrix;
	struct Node* next;		//pointer to next node
};

void myPush(struct Node **top, mat4 tempModelMatrix)
{
	struct Node* node = NULL;
	node = (struct Node*)malloc(sizeof(struct Node));

	if (!node)
	{
		/*fopen_s(&gpFile, "Log.txt", "a+");
		fprintf(gpFile, "Heap OverFlow\n");
		fclose(gpFile);*/
		exit(1);
	}

	node->modelMatrix = tempModelMatrix;

	//Set Next pointer of new node
	node->next = *top;

	//update top pointer
	*top = node;
}

int isEmpty(struct Node* top)
{
	return top == NULL;
}

mat4 peek(struct Node *top)
{
	if (!isEmpty(top))
	{
		return top->modelMatrix;
	}
	else
	{
		exit(EXIT_FAILURE);
	}
}

mat4 myPop(struct Node** top)
{
	struct Node *node;
	mat4 tempMatrix = mat4::identity();
	if (*top == NULL)
	{
	
		exit(1);
	}



	node = *top;
	tempMatrix = node->modelMatrix;
	*top = (*top)->next;

	free(node);
	return tempMatrix;
}