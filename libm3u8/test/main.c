#include <stdio.h>
#include <m3u8.h>

int main()
{
	Playlist playlist;
	IndexStreamList idxlist;
	PlaylistRecords *it = NULL;

	printf("\nInitializing new playlist...");

	// Init playlist properties
	playlist.x_type = VOD;
	playlist.x_target_duration = 11;
	playlist.x_version = 3;
	playlist.x_media_sequence = 0;

	// Init playlist list
	if (playlist_records_init(&playlist) < 0) {
		fprintf(stderr, "\nError: Playlist could not be initialized!");
		return 0;
	}

	printf("\t... Success!");

	printf("\n\nPLAYLIST:\n=========\n\tX_TYPE:\t\t\t%s\n\tX_TARGET_DURATION:\t%d\n\tX_VERSION:\t\t%d\n\tX_MEDIA_SEQUENCE:\t%d",
		   playlist_type_str(playlist.x_type), playlist.x_target_duration, playlist.x_version, playlist.x_media_sequence);

	playlist_records_add(&playlist, 10.0f, "PL__AWESOME", "/media/pl1.ts");
	playlist_records_add(&playlist, 9.875f, "OMG_UBUNTU!", "/media/pl2.ts");

	printf("\n\tRecords:");
	for (it = playlist.head; it != NULL; it = it->next)
		printf("\n\t[INF]\n\t\tLength:\t%g\n\t\tName:\t%s\n\t\tPath:\t%s\n", it->record->length, it->record->name, it->record->path);

	printf("\n\nM3U file generation:\n=========================\n");
	playlist_write_to(&playlist, stdout);
	playlist_write(&playlist, "_test_playlist.m3u8");

	printf("\n\nDestroying playlist...");
	playlist_records_destroy(&playlist);
	printf("\t... Success!");

	printf("\n\nRebuilding playlist from [_test_playlist.m3u8]:\n=======================================\n");
	playlist_load(&playlist, "_test_playlist.m3u8");

	printf("\n\nPLAYLIST:\n=========\n\tX_TYPE:\t\t\t%s\n\tX_TARGET_DURATION:\t%d\n\tX_VERSION:\t\t%d\n\tX_MEDIA_SEQUENCE:\t%d",
		   playlist_type_str(playlist.x_type), playlist.x_target_duration, playlist.x_version, playlist.x_media_sequence);

	printf("\n\tRecords:");
	for (it = playlist.head; it != NULL; it = it->next)
		printf("\n\t[INF]\n\t\tLength:\t%g\n\t\tName:\t%s\n\t\tPath:\t%s\n", it->record->length, it->record->name, it->record->path);

	printf("\n\nDestroying rebuilt playlist...");
	playlist_records_destroy(&playlist);
	printf("\t... Success!");

	printf("\n\nM3U index file generation\n===========================\n\n");
	index_stream_list_init(&idxlist);

	index_stream_list_add(&idxlist, 1, 41457, "mp4a.40.2", "/media/01.ts");
	index_stream_list_add(&idxlist, 1, 232370, "mp4a.40.2, avc1.4d4015", "/media/02.ts");

	printf("Saving M3U index file to [_index.m3u8]\n\n");
	index_stream_list_write(&idxlist, "_index.m3u8");
	index_stream_list_write_to(&idxlist, stdout);

	index_stream_list_destroy(&idxlist);

	printf("\n\nDestroying M3U index list");

	printf("\n\nBuilding M3U index file from [_index.m3u8]\n\n");

	index_stream_list_load(&idxlist, "_index.m3u8");
	index_stream_list_write_to(&idxlist, stdout);

	index_stream_list_destroy(&idxlist);
	printf("\n\nDestroying built M3U index list");

	printf("\n\nExiting...!\n");

	return 0;
}
