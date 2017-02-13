#include "Quantizer.h"
#include <climits>
#include <cstring>

#define NULL 0	

Quantize::Quantize(unsigned int nMaxColours, unsigned int nColourBits)
	: m_pTree(NULL), m_nLeafCount(0), m_nMaxColours(nMaxColours), m_prgb(NULL)
{
	//if(m_nColourBits < 0 || m_nColourBits > 8)
	//{
	//}

	for(int i = 0; i <= (int) nColourBits; i++)
		m_pReducibleNodes[i] = NULL;

	m_nColourBits = nColourBits;
}

 Quantize::~Quantize ()
 {
	if(m_pTree != NULL)
		DeleteTree(&m_pTree);
 }

 void Quantize::ProcessImage(unsigned char *pImage, unsigned int nImageSize)
 {
	for(unsigned int i = 0; i < nImageSize;)
	{
		unsigned char r = pImage[i++];
		unsigned char g = pImage[i++];
		unsigned char b = pImage[i++];

		AddColour(&m_pTree, r, g, b, m_nColourBits, 0, &m_nLeafCount,
			m_pReducibleNodes);

		while(m_nLeafCount > m_nMaxColours)
		{
			ReduceTree(m_nColourBits, &m_nLeafCount,
				m_pReducibleNodes);
		}
	}

	m_prgb = new RGB[m_nLeafCount];

	unsigned int nIndex = 0;
	GetPaletteColours(m_pTree, &nIndex);
 }

 void Quantize::AddColour(Node **ppNode, unsigned char r, unsigned char g, unsigned char b,
	 unsigned int nColourBits, unsigned int nLevel, unsigned int *pLeafCount, Node **pReducibleNodes)
 {
	static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	// If the node doesn't exist, create it.
	if(*ppNode == NULL)
	{
		*ppNode = CreateNode(nLevel, nColourBits, pLeafCount,
			pReducibleNodes);
	}

	// Update color information if it's a leaf node.
	if((*ppNode)->bIsLeaf)
	{
         (*ppNode)->nPixelCount++;
         (*ppNode)->nRedSum += r;
         (*ppNode)->nGreenSum += g;
         (*ppNode)->nBlueSum += b;
	}
	// Recurse a level deeper if the node is not a leaf.
	else
	{
		int shift = 7 - nLevel;
		int nIndex =	((	(r & mask[nLevel]) >> shift) << 2) |
						((	(g & mask[nLevel]) >> shift) << 1) |
						(	(b & mask[nLevel]) >> shift);

		AddColour(&((*ppNode)->pChild[nIndex]), r, g, b, nColourBits,
			nLevel + 1, pLeafCount, pReducibleNodes);
	}
 }

Quantize::Node *Quantize::CreateNode(unsigned int nLevel, unsigned int nColourBits, unsigned int *pLeafCount,
	Node **pReducibleNodes)
{
	Node *pNode = new Node;
	memset(pNode, 0, sizeof(Node));

	pNode->nLevel = nLevel;

	pNode->bIsLeaf = (nLevel == nColourBits);
	if(pNode->bIsLeaf)
		(*pLeafCount)++;
	else
	{
		pNode->pNext = pReducibleNodes[nLevel];
		pReducibleNodes[nLevel] = pNode;
	}
   
	return pNode;
}

void Quantize::ReduceTree(unsigned int nColourBits, unsigned int *pLeafCount,
         Node **pReducibleNodes)
 {
     // Find the deepest level containing at least one reducible node.
	 int i;
     for(i = nColourBits - 1; (i > 0) && (pReducibleNodes[i] == NULL); i--);
 
     // Reduce the node most recently added to the list at level i.
     Node *pNode = pReducibleNodes[i];
     pReducibleNodes[i] = pNode->pNext;
 
     unsigned int nRedSum = 0;
     unsigned int nGreenSum = 0;
     unsigned int nBlueSum = 0;
     unsigned int nChildren = 0;
 
     for(i = 0; i < 8; i++)
	 {
         if(pNode->pChild[i] != NULL)
		 {
             nRedSum += pNode->pChild[i]->nRedSum;
             nGreenSum += pNode->pChild[i]->nGreenSum;
             nBlueSum += pNode->pChild[i]->nBlueSum;
             pNode->nPixelCount += pNode->pChild[i]->nPixelCount;

			 delete pNode->pChild[i];
			 pNode->pChild[i] = NULL;
			 nChildren++;
         }
     }
 
     pNode->bIsLeaf = true;
     pNode->nRedSum = nRedSum;
     pNode->nGreenSum = nGreenSum;
     pNode->nBlueSum = nBlueSum;

     *pLeafCount -= (nChildren - 1);
 }


void Quantize::DeleteTree (Node **ppNode)
{
     for(int i = 0; i < 8; i++)
	 {
         if ((*ppNode)->pChild[i] != NULL)
             DeleteTree (&((*ppNode)->pChild[i]));
     }
	
	 delete[] *ppNode;
	 *ppNode = NULL;
}

void Quantize::GetPaletteColours(Node *pTree, unsigned int *pIndex)
{
	if(pTree->bIsLeaf)
	{
		m_prgb[*pIndex].red =
             (unsigned char) ((pTree->nRedSum) / (pTree->nPixelCount));
        m_prgb[*pIndex].green =
             (unsigned char) ((pTree->nGreenSum) / (pTree->nPixelCount));
        m_prgb[*pIndex].blue =
             (unsigned char) ((pTree->nBlueSum) / (pTree->nPixelCount));

		pTree->nIndex = *pIndex;
        (*pIndex)++;
    }
    else
	{
        for(int i = 0; i < 8; i++)
		{
            if(pTree->pChild[i])
                GetPaletteColours(pTree->pChild[i], pIndex);
        }
	}
}

void Quantize::GetPaletteIndex(Node *pTree, unsigned char r, unsigned char g, unsigned char b,
	 unsigned int nLevel, unsigned int *pIndex)
{
	static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	if(pTree->bIsLeaf)
		*pIndex = pTree->nIndex;
	// Recurse a level deeper if the node is not a leaf.
	else
	{
		int shift = 7 - nLevel;
		int nIndex =	((	(r & mask[nLevel]) >> shift) << 2) |
						((	(g & mask[nLevel]) >> shift) << 1) |
						(	(b & mask[nLevel]) >> shift);

		if (r != g && r != b && g != b) {
			shift++;
		}

		GetPaletteIndex(pTree->pChild[nIndex], r, g, b,
			nLevel + 1, pIndex);
	}
 }

 unsigned int Quantize::GetColourCount()
 {
     return m_nLeafCount;
 }
 
 RGB *Quantize::GetColourTable()
 {
	return m_prgb;
 }

unsigned int Quantize::GetColourIndex(unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int nIndex = 0;
	GetPaletteIndex(m_pTree, r, g, b, 0, &nIndex);

	return nIndex;
}
