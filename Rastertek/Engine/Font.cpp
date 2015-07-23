///////////////////////////////////////////////////////////////////////////////
// Filename: Font.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Font.h"


Font::Font()
{
	this->m_Font = nullptr;
	this->m_Texture = nullptr;
	this->m_charactersCount = 95;//The size of the array is set to 95 as that is the number of characters in the texture and hence the number of indexes in the fontdata.txt file.
}

Font::Font(const Font& other)
{
}


Font::~Font()
{
}

bool Font::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;

	//Load in the texture file containing the font data
	result = Font::LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	//Load the texture that has the font characters on it
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

void Font::Shutdown()
{
	//Release the font texture
	ReleaseTexture();

	//Release the font data
	ReleaseFontData();
}

bool Font::LoadFontData(char* fontFilename)
{
	ifstream fIn;
	char tmp;

	//Create the font spacing buffer
	this->m_Font = new FontType[this->m_charactersCount]; 
	if (!this->m_Font)
	{
		return false;
	}

	//Read in the font size and spacing between chars
	fIn.open(fontFilename);
	if (fIn.fail())
	{
		return false;
	}

	//Read in the characters for text
	for (int i = 0; i < this->m_charactersCount; i++)
	{
		fIn.get(tmp);
		while (tmp != ' ')
		{
			fIn.get(tmp);
		}
		fIn.get(tmp);
		while (tmp != ' ')
		{
			fIn.get(tmp);
		}
		fIn >> this->m_Font[i].left >> this->m_Font[i].right >> this->m_Font[i].size;
	}
	//Close the file
	fIn.close();

	return true;
}

void Font::ReleaseFontData()
{
	//Release the font data array
	if (this->m_Font)
	{
		delete[] this->m_Font;
		this->m_Font = nullptr;
	}
}

bool Font::LoadTexture(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	//Create the texture object
	this->m_Texture = new Texture();
	if (!this->m_Texture)
	{
		return false;
	}

	//Initialize the texture object
	result = this->m_Texture->Initialize(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void Font::ReleaseTexture()
{
	if (this->m_Texture)
	{
		this->m_Texture->Shutdown();
		delete this->m_Texture;
		this->m_Texture = nullptr;
	}
}

ID3D11ShaderResourceView* Font::GetTexture()
{
	return this->m_Texture->GetTexture();
}

void Font::BuildVertexArray(void* vertices, char* sentence, D3DXVECTOR2 drawPosition)
{
	VertexType* vertexPtr;
	int numLetters;
	int index;
	int letter;

	//Coerce the input vertices into a VertexType structure
	vertexPtr = (VertexType*)vertices;

	//Get the number of letters in the sentence
	numLetters = (int)strlen(sentence);

	//Initialize the index to the vertex array
	index = 0;

	//Draw each letter onto a quad
	for (int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		//If the letter is a space then just move over three pixels
		if (letter == 0)
		{
			drawPosition.x += 3.0f;
		}
		else
		{
			//First triangle in quad
			vertexPtr[index].position = D3DXVECTOR3(drawPosition.x, drawPosition.y, 0.0f);								//Top Left
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawPosition.x + this->m_Font[letter].size), (drawPosition.y - 16), 0.0f);	//Bottom Right
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawPosition.x, (drawPosition.y - 16), 0.0f);							//Bottom Left
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].left, 1.0f);
			index++;

			//Second triangle in quad
			vertexPtr[index].position = D3DXVECTOR3(drawPosition.x, drawPosition.y, 0.0f);								//Top Left
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawPosition.x + this->m_Font[letter].size, drawPosition.y, 0.0f);	//Top Right
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawPosition.x + this->m_Font[letter].size, drawPosition.y - 16, 0.0f);	//Bottom Right
			vertexPtr[index].texture = D3DXVECTOR2(this->m_Font[letter].right, 1.0f);
			index++;

			//Update the x location for drawing by the size of the letter and one pixel
			drawPosition.x += this->m_Font[letter].size + 1.0f;
		}
	}
}