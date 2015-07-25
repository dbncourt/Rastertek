///////////////////////////////////////////////////////////////////////////////
// Filename: ModelList.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _MODELLIST_H_
#define _MODELLIST_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include <stdlib.h>
#include <time.h>


///////////////////////////////////////////////////////////////////////////////
// Class name: ModelList
///////////////////////////////////////////////////////////////////////////////

#pragma once
class ModelList
{
private:
	struct ModelInfoType
	{
		D3DXCOLOR color;
		D3DXVECTOR3 position;
	};

	int m_modelCount;
	ModelInfoType* m_ModelInfoList;

public:
	ModelList();
	ModelList(const ModelList& other);
	~ModelList();

	bool Initialize(int numModels);
	void Shutdown();

	int GetModelCount();
	void GetData(int index, D3DXVECTOR3& position, D3DXCOLOR& color);
};
#endif