#ifndef QUANTIZE_H_
#define QUANTIZE_H_

struct RGB
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

class Quantizer
{
protected:
	struct Node
	{
		bool is_leaf;			// True if node has no children

		unsigned int pixel_count;	// Number of pixels represented by this leaf
		unsigned int red_sum;		// Sum of red components
		unsigned int green_sum;		// Sum of green components
		unsigned int blue_sum;		// Sum of blue components

		unsigned int index;		// Colour table index

		Node* child[8];			// Pointer to child nodes
		Node* next;			// Pointer to next reducible node
	};

	Node* tree_;
	Node* reducible_nodes_[9];

	unsigned int leaf_count_;
	unsigned int max_colours_;
	unsigned int colour_bits_;

public:
	Quantizer(unsigned int max_colours, unsigned int colour_bits);
	virtual ~Quantizer();

	void ProcessImage(unsigned char* image, unsigned int image_size);

	unsigned int GetColourCount();
	void GetColourTable(RGB* rgb);

	unsigned int GetColourIndex(unsigned char r, unsigned char g, unsigned char b);

protected:
	void AddColour(Node** node, unsigned char r, unsigned char g, unsigned char b, unsigned int colour_bits,
		unsigned int level, unsigned int* leaf_count, Node** reducible_nodes);

	Node* CreateNode(unsigned int level, unsigned int colour_bits, unsigned int* leaf_count,
		Node** reducible_nodes);

	void ReduceTree(unsigned int colour_bits, unsigned int* leaf_count,
		Node** reducible_nodes);

	void DeleteTree(Node** node);

	void GetPaletteColours(Node* node, RGB* rgb, unsigned int* index);
	unsigned int GetPaletteIndex(Node* node, unsigned char r, unsigned char g, unsigned char b, unsigned int level);
};

#endif // QUANTIZE_H_

