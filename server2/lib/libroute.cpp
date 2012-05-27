extern "C" {
	float * get_path();
	void search(float x1, float y1,
				float x2, float y2);
	int get_size();
	void free_mem();
	float get_i(int i);
		
	int path_size;
	float * ptr;
	
	float * get_path() {
		return ptr;
	}
	
	void search(float x1, float y1,
				float x2, float y2) {
	
		ptr = new float[6];
		
		ptr[0] = x1;
		ptr[1] = y1;
		ptr[2] = x1 - 1000;
		ptr[3] = y1 - 500;
		ptr[4] = x2;
		ptr[5] = y2;

		path_size = 6;
	}

	int get_size() {
		return path_size;
	}
	
	float get_i(int i) {
		return ptr[i];
	}

	void free_mem() {
		delete[] ptr;
	}
}
