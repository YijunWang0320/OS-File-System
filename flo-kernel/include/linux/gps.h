struct gps_location {
	double latitude;
	double longitude;
	float  accuracy;  /* in meters */
};

struct gps_location local_kernel;