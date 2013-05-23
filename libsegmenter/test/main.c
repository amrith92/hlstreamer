#include <segmenter.h>
#include <stdio.h>

int main()
{
	Params p = {
		"/home/amrith92/Videos/dbh.flv",
		"/home/amrith92/Videos/Test/DBH/%d.ts",
		{ 960 /* width */, 540 /* height */, 100 /* key-int-max */, 910 /* bitrate */, 30 /* framerate */, 150 /* noise-reduction factor */},
		{ 22050 /* bitrate */, 2 /* channels */ }
	};

	int ret = segmenter_try(&p);

	switch(ret) {
	case 0:
		fprintf(stdout, "Completed Successfully! Output at: %s\n", p.out);
	break;
	case -128:
	case 100:
	case 101:
	case 102:
	case 200:
	case 201:
	case 202:
	case 203:
	case 204:
		fprintf(stderr, "Something went wrong. Unable to complete segmentation!\n");
	break;

	default:
		fprintf(stderr, "Unknown result! Looks like something isn't quite right!\n");
	}

	return 0;
}
