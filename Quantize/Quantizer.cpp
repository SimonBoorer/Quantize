#include "Quantizer.h"

Quantizer::Quantizer(unsigned int max_colours, unsigned int colour_bits)
	: tree_(nullptr), leaf_count_(0), max_colours_(max_colours)
{
	//if (colour_bits <= 8)
	//{
	//}

	for (int i = 0; i <= (int) colour_bits; i++)
		reducible_nodes_[i] = nullptr;

	colour_bits_ = colour_bits;
}

Quantizer::~Quantizer ()
{
	if (tree_ != nullptr)
		DeleteTree(&tree_);
}

void Quantizer::ProcessImage(uint8_t* pixels, unsigned int image_bytes)
{
	for (unsigned int i = 0; i < image_bytes;)
	{
		uint8_t r = pixels[i++];
		uint8_t g = pixels[i++];
		uint8_t b = pixels[i++];

		AddColour(&tree_, r, g, b, colour_bits_, 0, &leaf_count_,
			reducible_nodes_);

		while(leaf_count_ > max_colours_)
		{
			ReduceTree(colour_bits_, &leaf_count_,
				reducible_nodes_);
		}
	}
}

void Quantizer::AddColour(Node** node, uint8_t r, uint8_t g, uint8_t b,
	unsigned int colour_bits, unsigned int level, unsigned int* leaf_count, Node** reducible_nodes)
{
	static uint8_t mask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	// If the node doesn't exist, create it.
	if (*node == nullptr)
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
	Node* node = new Node{};

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
	for (i = colour_bits - 1; (i > 0) && (reducible_nodes[i] == nullptr); i--);

	// Reduce the node most recently added to the list at level i.
	Node* node = reducible_nodes[i];
	reducible_nodes[i] = node->next;

	unsigned int red_sum = 0;
	unsigned int green_sum = 0;
	unsigned int blue_sum = 0;
	unsigned int children = 0;

	for (i = 0; i < 8; i++)
	{
		if (node->child[i] != nullptr)
		{
			red_sum += node->child[i]->red_sum;
			green_sum += node->child[i]->green_sum;
			blue_sum += node->child[i]->blue_sum;
			node->pixel_count += node->child[i]->pixel_count;

			delete node->child[i];
			node->child[i] = nullptr;
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
		if ((*node)->child[i] != nullptr)
			DeleteTree(&((*node)->child[i]));
	}
	
	delete[] *node;
	*node = nullptr;
}

void Quantizer::GetPaletteColours(Node* node, Palette& palette, unsigned int* index)
{
	if (node->is_leaf)
	{
		Colour colour =
		{
			static_cast<uint8_t>(node->red_sum / node->pixel_count),
			static_cast<uint8_t>(node->green_sum / node->pixel_count),
			static_cast<uint8_t>(node->blue_sum / node->pixel_count),
		};
		palette.push_back(colour);

		node->index = *index;
		(*index)++;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (node->child[i])
				GetPaletteColours(node->child[i], palette, index);
		}
	}
}

Palette Quantizer::GetColourPalette()
{
	Palette palette;
	palette.reserve(leaf_count_);
    unsigned int index = 0;
    GetPaletteColours(tree_, palette, &index);
	return palette;
}
