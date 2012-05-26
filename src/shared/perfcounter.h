#pragma once 

struct perf_counter
{
	perf_counter()
	{
		QueryPerformanceFrequency(&freq_);
		reset();
	}

	void reset()
	{
		QueryPerformanceCounter(&tick_);
	}

	double time() const 
	{
		LARGE_INTEGER cur_tick;
		QueryPerformanceCounter(&cur_tick);

		return (cur_tick.QuadPart - tick_.QuadPart) / (double) freq_.QuadPart;
	}

	double time_ms() const 
	{
		return 1000. * time();
	}

private:
	LARGE_INTEGER freq_;
	LARGE_INTEGER tick_;
};
