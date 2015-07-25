#include "ModelList.h"


ModelList::ModelList()
{
	this->m_ModelInfoList = nullptr;
}

ModelList::ModelList(const ModelList& other)
{
}

ModelList::~ModelList()
{
}

bool ModelList::Initialize(int numModels)
{
	D3DXCOLOR color;

	//Store the number of models
	this->m_modelCount = numModels;

	//Create a list array of the model information
	this->m_ModelInfoList = new ModelInfoType[this->m_modelCount];
	if (!this->m_ModelInfoList)
	{
		return false;
	}

	//Seed the random generator with the current time
	srand((UINT)time(nullptr));

	//Go through all the models and randomly generate the model color and position
	for (int i = 0; i < this->m_modelCount; i++)
	{
		//Generate a random color for the model
		color = D3DXCOLOR(
			(FLOAT)rand() / RAND_MAX, //red
			(FLOAT)rand() / RAND_MAX, //green
			(FLOAT)rand() / RAND_MAX, //blue
			1.0f
			);

		this->m_ModelInfoList[i].color = color;

		//Generate a random position in front of the viewer for the model
		this->m_ModelInfoList[i].position = D3DXVECTOR3(
			(((FLOAT)rand() - (FLOAT)rand()) / RAND_MAX)*10.0f,
			(((FLOAT)rand() - (FLOAT)rand()) / RAND_MAX)*10.0f,
			((((FLOAT)rand() - (FLOAT)rand()) / RAND_MAX)*10.0f) + 5.0f
			);
	}
	return true;
}

void ModelList::Shutdown()
{
	//Release the ModelInfoList
	if (this->m_ModelInfoList)
	{
		delete[] this->m_ModelInfoList;
		this->m_ModelInfoList = nullptr;
	}
}

int ModelList::GetModelCount()
{
	return this->m_modelCount;
}

void ModelList::GetData(int index, D3DXVECTOR3& position, D3DXCOLOR& color)
{
	position = this->m_ModelInfoList[index].position;
	color = this->m_ModelInfoList[index].color;
}