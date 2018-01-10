#include "Quantizer.h"
#include <climits>
#include <cstring>

Quantizer::Quantizer(unsigned int max_colours, unsigned int colour_bits)
	: tree_(NULL), leaf_count_(0), max_colours_(max_colours)
{
	//if (colour_bits <= 8)
	//{
	//}

	for (int i = 0; i <= (int) colour_bits; i++)
		reducible_nodes_[i] = NULL;

	colour_bits_ = colour_bits;
}

Quantizer::~Quantizer ()
{
	if (tree_ != NULL)
		DeleteTree(&tree_);
}

void Quantizer::ProcessImage(unsigned char* pixels, unsigned int image_bytes)
{
	for (unsigned int i = 0; i < image_bytes;)
	{
		unsigned char r = pixels[i++];
		unsigned char g = pixels[i++];
		unsigned char b = pixels[i++];

		AddColour(&tree_, r, g, b, colour_bits_, 0, &leaf_count_,
			reducible_nodes_);

		while(leaf_count_ > max_colours_)
		{
			ReduceTree(colour_bits_, &leaf_count_,
				reducible_nodes_);
		}
	}
}

void Quantizer::AddColour(Node** node, unsigned char r, unsigned char g, unsigned char b,
	unsigned int colour_bits, unsigned int level, unsigned int* leaf_count, Node** reducible_nodes)
{
	static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	// If the node doesn't exist, create it.
	if (*node == NULL)
	{
		*node = CreateNode(level, colour_bits, leaf_count,
			reducible_nodes);
	}

	// Update colour information if it's a leaf node.
	if ((*node)->is_leaf)
	{
		(*node)->pixel_count++;
		(*node)->red_sum += r;
		(*node)->green_sum += g;
		(*node)->blue_sum += b;
	}
	// Recurse a level deeper if the node is not a leaf.
	else
	{
		int shift = 7 - level;
		int index = (((r & mask[level]) >> shift) << 2) |
			(((g & mask[level]) >> shift) << 1) |
			((b & mask[level]) >> shift);

		AddColour(&((*node)->child[index]), r, g, b, colour_bits,
			level + 1, leaf_count, reducible_nodes);
	}
}

Quantizer::Node* Quantizer::CreateNode(unsigned int level, unsigned int colour_bits, unsigned int* leaf_count,
	Node** reducible_nodes)
{
	Node* node = new Node;
	memset(node, 0, sizeof(Node));

	node->is_leaf = (level == colour_bits);
	if (node->is_leaf)
		(*leaf_count)++;
	else
	{
		node->next = reducible_nodes[level];
		reducible_nodes[level] = node;
	}

	return node;
}

void Quantizer::ReduceTree(unsigned int colour_bits, unsigned int* leaf_count,
	Node** reducible_nodes)
{
	// Find the deepest level containing at least one reducible node.
	int i;
	for (i = colour_bits - 1; (i > 0) && (reducible_nodes[i] == NULL); i--);

	// Reduce the node most recently added to the list at level i.
	Node* node = reducible_nodes[i];
	reducible_nodes[i] = node->next;

	unsigned int red_sum = 0;
	unsigned int green_sum = 0;
	unsigned int blue_sum = 0;
	unsigned int children = 0;

	for (i = 0; i < 8; i++)
	{
		if (node->child[i] != NULL)
		{
			red_sum += node->child[i]->red_sum;
			green_sum += node->child[i]->green_sum;
			blue_sum += node->child[i]->blue_sum;
			node->pixel_count += node->child[i]->pixel_count;

			delete node->child[i];
			node->child[i] = NULL;
			children++;
		}
	}

	node->is_leaf = true;
	node->red_sum = red_sum;
	node->green_sum = green_sum;
	node->blue_sum = blue_sum;

	*leaf_count -= (children - 1);
}


void Quantizer::DeleteTree (Node** node)
{
	for (int i = 0; i < 8; i++)
	{
		if ((*node)->child[i] != NULL)
			DeleteTree (&((*node)->child[i]));
	}
	
	delete[] *node;
	*node = NULL;
}

void Quantizer::GetPaletteColours(Node* node, RGB* rgb, unsigned int* index)
{
	if (node->is_leaf)
	{
		rgb[*index].red =
			(unsigned char) ((node->red_sum) / (node->pixel_count));
		rgb[*index].green =
			(unsigned char) ((node->green_sum) / (node->pixel_count));
		rgb[*index].blue =
			(unsigned char) ((node->blue_sum) / (node->pixel_count));

		node->index = *index;
		(*index)++;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (node->child[i])
				GetPaletteColours(node->child[i], rgb, index);
		}
	}
}

unsigned int Quantizer::GetPaletteIndex(Node* node, unsigned char r, unsigned char g, unsigned char b,
	 unsigned int level)
{
	static unsigned char mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	unsigned int palette_index = node->index;

	// Recurse a level deeper if the node is not a leaf.
	if (!node->is_leaf)
	{
		int shift = 7 - level;
		int index = (((r & mask[level]) >> shift) << 2) |
			(((g & mask[level]) >> shift) << 1) |
			((b & mask[level]) >> shift);

		palette_index = GetPaletteIndex(node->child[index], r, g, b,
			level + 1);
	}

	return palette_index;
}

unsigned int Quantizer::GetColourCount()
{
	return leaf_count_;
}

void Quantizer::GetColourTable(RGB* rgb)
{
        unsigned int index = 0;
        GetPaletteColours(tree_, rgb, &index);
}

unsigned int Quantizer::GetColourIndex(unsigned char r, unsigned char g, unsigned char b)
{
	return GetPaletteIndex(tree_, r, g, b, 0);
}

