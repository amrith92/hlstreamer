#include <stdio.h>
#include <m3u8.h>

int main()
{
	Playlist playlist;
	PlaylistRecords *it = NULL;

	printf("\nInitializing new playlist...");

	// Init playlist properties
	playlist.x_type = VOD;
	playlist.x_target_duration = 11;
	playlist.x_version = 3;
	playlist.x_media_sequence = 0;

	// Init playlist list
	if (record_list_init(&playlist.head) < 0) {
		fprintf(stderr, "\nError: Playlist could not be initialized!");
		return 0;
	}

	playlist.cur = playlist.head;

	printf("\t... Success!");

	printf("\n\nPLAYLIST:\n=========\n\tX_TYPE:\t\t\t%s\n\tX_TARGET_DURATION:\t%d\n\tX_VERSION:\t\t%d\n\tX_MEDIA_SEQUENCE:\t%d",
		   playlist_type_str(playlist.x_type), playlist.x_target_duration, playlist.x_version, playlist.x_media_sequence);

	record_list_add_more(&playlist.cur, 10.0f, "PL__AWESOME", "/media/pl1.ts");
	record_list_add_more(&playlist.cur, 9.875f, "OMG_UBUNTU!", "/media/pl2.ts");

	printf("\n\tRecords:");
	for (it = playlist.head; it != NULL; it = it->next)
		printf("\n\t[INF]\n\t\tLength:\t%g\n\t\tName:\t%s\n\t\tPath:\t%s\n", it->record->length, it->record->name, it->record->path);

	printf("\n\nM3U file generation:\n=========================\n");
	playlist_write_to(&playlist, stdout);
	playlist_write(&playlist, "_test_playlist.m3u8");

	printf("\n\nDestroying playlist...");
	record_list_destroy(&playlist.head);
	printf("\t... Success!");

	playlist.head = playlist.cur = NULL;
	printf("\n\nRebuilding playlist from [_test_playlist.m3u8]:\n=======================================\n");
	playlist_load(&playlist, "_test_playlist.m3u8");

	printf("\n\nPLAYLIST:\n=========\n\tX_TYPE:\t\t\t%s\n\tX_TARGET_DURATION:\t%d\n\tX_VERSION:\t\t%d\n\tX_MEDIA_SEQUENCE:\t%d",
		   playlist_type_str(playlist.x_type), playlist.x_target_duration, playlist.x_version, playlist.x_media_sequence);

	printf("\n\tRecords:");
	for (it = playlist.head; it != NULL; it = it->next)
		printf("\n\t[INF]\n\t\tLength:\t%g\n\t\tName:\t%s\n\t\tPath:\t%s\n", it->record->length, it->record->name, it->record->path);

	printf("\n\nDestroying rebuilt playlist...");
	record_list_destroy(&playlist.head);
	printf("\t... Success!");

	printf("\n\nExiting...!\n");

	return 0;
}
