#ifndef CNODE_H
#define CNODE_H

class cNode
{
	public:
		// Methods.
		cNode();
		cNode(int a);
		cNode(int a, int b, int c, int d);

		void setI(int a);
		void setFGH(int a, int b);

		int getI();
		int getF();
		int getH();
		int getG();

	private:
		// Attributes.
		int i;			// index of tile
		int f, g, h;	// f/g/h costs for pathing
};

#endif CNODE_H