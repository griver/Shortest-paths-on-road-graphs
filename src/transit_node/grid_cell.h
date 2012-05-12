#ifndef GRID_CELL_H
#define GRID_CELL_H

namespace TNR {
	struct grid_cell {
		int x;
		int y;
		grid_cell(int x, int y): x(x), y(y) {}
		grid_cell(): x(0), y(0) {}
	};

	bool operator<(grid_cell const &a, grid_cell const &b) {
		if(a.x < b.x)
			return true;
		if(a.x > b.x)
			return false;
		if(a.x == b.x)
			return (a.y < b.y);
	}

	bool operator == (grid_cell const &a, grid_cell const &b) {
		return(a.x == b.x) && (a.y == b.y);
	}

	bool operator != (grid_cell const &a, grid_cell const &b) {
		return !(a == b);
	}

}


#endif