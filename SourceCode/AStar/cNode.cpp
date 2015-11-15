#include "cNode.h"

cNode::cNode(){}

cNode::cNode(int a, int b, int c, int d)
{
	i = a;
	f = b;
	g = c;
	h = d;
}

cNode::cNode(int a)
{
	i = a;
}

void cNode::setI(int a)
{
	i = a;
}

void cNode::setFGH(int a, int b)
{
	f = a + b;
	g = a;
	h = b;

	i = g;
}

int cNode::getI()
{
	return i;
}

int cNode::getF()
{
	return f;
}

int cNode::getH()
{
	return h;
}

int cNode::getG()
{
	return g;
}