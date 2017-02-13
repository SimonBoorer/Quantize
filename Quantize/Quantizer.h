#ifndef _QUANTIZE_H_
#define _QUANTIZE_H_

// Wicked Code, MSJ October 1997
// http://www.microsoft.com/msj/1097/wicked1097.aspx
// http://www.microsoft.com/msj/1097/wickedtextfigs.htm#fig3

// Optimizing Color Quantization for ASP.NET Images
// http://msdn.microsoft.com/en-us/library/aa479306.aspx
 
// Dr. Dobb's | Color Quantization using Octrees | January 1, 1996
// http://git.hashcollision.org/projects/plt-misc/octree-quantization/article.pdf

struct RGB
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

class Quantize
{
protected:
	struct Node
	{
		bool bIsLeaf;				// True if node has no children

		unsigned int nPixelCount;	// Number of pixels represented by this leaf
		unsigned int nRedSum;		// Sum of red components
		unsigned int nGreenSum;		// Sum of green components
		unsigned int nBlueSum;		// Sum of blue components

		unsigned int nLevel;
		unsigned int nIndex;		// Colour table index

		Node *pChild[8];			// Pointer to child nodes
		Node *pNext;				// Pointer to next reducible node
	 };

     Node *m_pTree;
     Node *m_pReducibleNodes[9];

	 unsigned int m_nLeafCount;
     unsigned int m_nMaxColours;
	 unsigned int m_nColourBits;

	 RGB *m_prgb;
 
 public:
     Quantize(unsigned int nMaxColours, unsigned int nColourBits);
     virtual ~Quantize();

     void ProcessImage(unsigned char *pImage, unsigned int nImageSize);

     unsigned int GetColourCount();
     RGB *GetColourTable();

	 unsigned int GetColourIndex(unsigned char r, unsigned char g, unsigned char b);
 
 protected:
     void AddColour(Node **ppNode, unsigned char r, unsigned char g, unsigned char b, unsigned int nColourBits,
		 unsigned int nLevel, unsigned int *pLeafCount, Node **pReducibleNodes);

     Node *CreateNode(unsigned int nLevel, unsigned int nColourBits, unsigned int *pLeafCount,
         Node **pReducibleNodes);

     void ReduceTree(unsigned int nColourBits, unsigned int *pLeafCount,
         Node** pReducibleNodes);

     void DeleteTree(Node **ppNode);

     void GetPaletteColours(Node *pTree, unsigned int *pIndex);
	 void GetPaletteIndex(Node *pTree, unsigned char r, unsigned char g, unsigned char b, unsigned int nLevel, unsigned int *pIndex);
};

#endif // _QUANTIZE_H_
