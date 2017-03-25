#pragma once
#include "dos.h"

struct Node
{
	std::vector<std::shared_ptr<Node>> children;
};

struct Tree
{
	Tree()
	{
		root = std::make_shared<Node>();
	}

	std::shared_ptr<Node> addNode(std::shared_ptr<Node> parent)
	{
		auto node = std::make_shared<Node>();
		parent->children.push_back(node);
	}

	std::shared_ptr<Node> root;
};