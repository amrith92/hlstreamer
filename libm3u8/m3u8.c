#include "m3u8.h"
#include <stdlib.h>
#include <string.h>

/**
 *	Playlist types number
 */
#define PLAYLIST_TYPES_COUNT	5

/**
 *	Playlist types string definitions
 */
static const char *_playlist_type_defs[PLAYLIST_TYPES_COUNT] = {
	0,
	"VOD",
	"EVENT",
	"LIVE",
	"VARIANT"
};

int8_t inf_init(InfRecord **record)
{
	*record = (InfRecord *) malloc(sizeof(InfRecord));

	if (*record == NULL)
		return -1;

	(*record)->name = NULL;
	(*record)->path = NULL;
	(*record)->length = 0.0;

	return 0;
}

int8_t inf_set_length(InfRecord **record, const float length)
{
	if (*record == NULL)
		return -1;

	(*record)->length = length;

	return 0;
}

int8_t inf_set_name(InfRecord **record, const char *name)
{
	size_t len = 0;
	if (*record == NULL)
		return -1;

	if ((*record)->name != NULL) {
		free ((*record)->name);

		if ((*record)->name != NULL)
			return -2;
	}

	len = strlen(name);
	if (len > 0) {
		(*record)->name = (char *) malloc(len + 1);
		memset((*record)->name, 0, len + 1);

		if ((*record)->name == NULL)
			return -3;
		strcpy((*record)->name, name);
	}

	return 0;
}

int8_t inf_set_path(InfRecord **record, const char *path)
{
	if (*record == NULL)
		return -1;

	if ((*record)->path != NULL) {
		free ((*record)->path);

		if ((*record)->path != NULL)
			return -2;
	}

	(*record)->path = (char *) malloc(strlen(path) + 1);

	if ((*record)->path == NULL)
		return -3;
	strcpy((*record)->path, path);

	return 0;
}

int8_t inf_destroy(InfRecord **record)
{
	if (*record == NULL)
		return -1;

	if ((*record)->name != NULL)
		free ((*record)->name);
	if ((*record)->path != NULL)
		free ((*record)->path);

	free (*record);
	return 0;
}

int8_t record_list_init(PlaylistRecords **head)
{
	*head = (PlaylistRecords *) malloc(sizeof(PlaylistRecords));

	if (*head == NULL)
		return -1;

	(*head)->next = NULL;
	(*head)->prev = NULL;
	(*head)->record = NULL;

	return 0;
}

int8_t record_list_add(PlaylistRecords **cur, InfRecord *data)
{
	if (*cur == NULL)
		return -1;

	if ((*cur)->record != NULL) {
		(*cur)->next = (PlaylistRecords *) malloc(sizeof(PlaylistRecords));

		if ((*cur)->next == NULL)
			return -2;

		// Init INF-RECORD
		inf_init(&(*cur)->next->record);
		inf_set_length(&(*cur)->next->record, data->length);
		inf_set_name(&(*cur)->next->record, data->name);
		inf_set_path(&(*cur)->next->record, data->path);

		(*cur)->next->next = NULL;
		(*cur)->next->prev = *cur;
		*cur = (*cur)->next;
		(*cur)->prev->next = *cur;
	} else {
		// Init INF-RECORD
		inf_init(&(*cur)->record);
		inf_set_length(&(*cur)->record, data->length);
		inf_set_name(&(*cur)->record, data->name);
		inf_set_path(&(*cur)->record, data->path);
	}

	return 0;
}

int8_t record_list_add_more(PlaylistRecords **cur, const float length, const char *name, const char *path)
{
	if (*cur == NULL)
		return -1;

	if ((*cur)->record == NULL) {
		// Init INF-RECORD
		inf_init(&(*cur)->record);
		inf_set_length(&(*cur)->record, length);
		inf_set_name(&(*cur)->record, name);
		inf_set_path(&(*cur)->record, path);
	} else {
		(*cur)->next = (PlaylistRecords *) malloc(sizeof(PlaylistRecords));

		if ((*cur)->next == NULL)
			return -2;

		// Init INF-RECORD
		inf_init(&(*cur)->next->record);
		inf_set_length(&(*cur)->next->record, length);
		inf_set_name(&(*cur)->next->record, name);
		inf_set_path(&(*cur)->next->record, path);

		(*cur)->next->next = NULL;
		(*cur)->next->prev = *cur;
		*cur = (*cur)->next;
		(*cur)->prev->next = *cur;
	}

	return 0;
}

int8_t record_list_remove_at(PlaylistRecords **head, uint16_t index)
{
	PlaylistRecords *tmp = *head;
	uint16_t i;

	if (*head == NULL)
		return -1;

	for (i = 0; i < index && tmp; ++i)
		tmp = tmp->next;

	if (tmp != NULL) {
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;

		free (&tmp->record);
		free (tmp);
	}

	return 0;
}

int8_t record_list_remove_first_n(PlaylistRecords **head, uint16_t n)
{
	PlaylistRecords *tmp = *head, *cur = NULL;
	uint16_t i;

	if (*head == NULL)
		return -1;

	for (i = 0; i < n && tmp; ++i) {
		cur = tmp;
		tmp = tmp->next;
		free (&cur->record);
		free (cur);
	}

	if (tmp != NULL) {
		*head = tmp->next;
	}

	return 0;
}

int8_t record_list_destroy(PlaylistRecords **head)
{
	PlaylistRecords *tmp;

	if (*head == NULL)
		return -1;

	while (*head) {
		tmp = *head;
		*head = (*head)->next;
		inf_destroy(&tmp->record);
		free (tmp);
	}

	return 0;
}

XPlaylistType playlist_type_from(const char *str)
{
	size_t i = 0;
	for (i = 1; i < PLAYLIST_TYPES_COUNT; ++i) {
		if (!strcasecmp(_playlist_type_defs[i], str))
			return (XPlaylistType) i;
	}

	return DEFAULT;
}

const char *playlist_type_str(const XPlaylistType type)
{
	switch (type) {
	case VOD: return _playlist_type_defs[1];
	case EVENT: return _playlist_type_defs[2];
	case LIVE: return _playlist_type_defs[3];
	case VARIANT: return _playlist_type_defs[4];
	case DEFAULT: return _playlist_type_defs[0];
	}

	return _playlist_type_defs[0];
}

int8_t playlist_write(Playlist *playlist, const char *filename)
{
	FILE *out = NULL;
	int8_t ret = 0;

	if (filename == NULL)
		return -1;

	out = fopen(filename, "w");
	if (out == NULL) {
		return -2;
	}

	ret = playlist_write_to(playlist, out);

	fclose(out);

	return ret;
}

int8_t playlist_write_to(Playlist *playlist, FILE *out)
{
	PlaylistRecords *it = NULL;

	if (out == NULL) {
		return -2;
	}

	// Begin write
	fprintf(out, "#EXTM3U\n#EXT-X-TARGETDURATION:%d\n#EXT-X-VERSION:%d\n#EXT-X-MEDIA-SEQUENCE:%d\n#EXT-X-PLAYLIST-TYPE:%s",
			playlist->x_target_duration, playlist->x_version, playlist->x_media_sequence, playlist_type_str(playlist->x_type));

	for (it = playlist->head; it != NULL; it = it->next)
		fprintf(out, "\n#EXTINF:%g, %s\n%s", it->record->length, it->record->name, it->record->path);

	if (playlist->x_type != LIVE)
		fprintf(out, "\n#EXT-X-ENDLIST");

	return 0;
}

int8_t playlist_load(Playlist *playlist, const char *filename)
{
	FILE *in = NULL;
	int8_t ret = 0;

	if (playlist == NULL)
		return -1;
	if (filename == NULL)
		return -2;

	in = fopen(filename, "r");

	if (in == NULL)
		return -3;

	ret = playlist_load_from(playlist, in);

	fclose(in);

	return ret;
}

int8_t playlist_load_from(Playlist *playlist, FILE *in)
{
	char buffer[BUFSIZ], *tok, *name = NULL;
	float length = 0;
	int delay = 0;

	if (in == NULL)
		return -1;

	// First line MUST be #EXTM3U
	fscanf(in, "%s", buffer);
	if (strcasecmp(buffer, "#EXTM3U"))
		return 7;

	while (!feof(in)) {
		memset(buffer, 0, BUFSIZ);
		fgets(buffer, BUFSIZ, in);

		if (!strcasecmp(buffer, "#EXT-X-ENDLIST")) {
			break;
		} else {
			if (delay) {
				delay = 0;
				tok = strtok(buffer, "\n ");
				if (playlist->head == NULL) {
					record_list_init(&playlist->head);
					playlist->cur = playlist->head;
				}
				record_list_add_more(&playlist->cur, length, name, tok);
				free (name);
				continue;
			}

			tok = strtok(buffer, ": ");

			while (tok != NULL) {
				if (!strcasecmp(tok, "#EXTINF")) {
					// Grab length
					tok = strtok(NULL, ", \n");
					length = atof(tok);
					// Grab name (if exists)
					tok = strtok(NULL, ", \n");
					if (tok != NULL) {
						name = (char *) malloc((strlen(tok) * sizeof(char)) + 1);
						strcpy(name, tok);
					}
					delay = 1;
				} else if (!strcasecmp(tok, "#EXT-X-TARGETDURATION")) {
					tok = strtok(NULL, ": ");
					playlist->x_target_duration = atoi(tok);
				} else if (!strcasecmp(tok, "#EXT-X-VERSION")) {
					tok = strtok(NULL, ": ");
					playlist->x_version = atoi(tok);
				} else if (!strcasecmp(tok, "#EXT-X-MEDIA-SEQUENCE")) {
					tok = strtok(NULL, ": ");
					playlist->x_media_sequence = atoi(tok);
				} else if (!strcasecmp(tok, "#EXT-X-PLAYLIST-TYPE")) {
					tok = strtok(NULL, ": \n");
					playlist->x_type = playlist_type_from(tok);
				}

				if (tok == NULL)
					break;
				tok = strtok(NULL, ": ");
			}
		}
	}

	return 0;
}

int8_t index_stream_list_init(IndexStreamRecord **head)
{
	if (*head != NULL)
		return -1;

	*head = (IndexStreamRecord *) malloc(sizeof(IndexStreamRecord));

	if (*head == NULL)
		return -2;

	(*head)->next = NULL;
	(*head)->prev = NULL;
	(*head)->path = NULL;
	(*head)->codecs = NULL;
	(*head)->program_id = 1;
	(*head)->bandwidth = 0;

	return 0;
}

int8_t index_stream_list_set_program_id(IndexStreamRecord **cur, int8_t program_id)
{
	if (*cur == NULL)
		return -1;

	(*cur)->program_id = program_id;

	return 0;
}

int8_t index_stream_list_set_bandwidth(IndexStreamRecord **cur, uint64_t bandwidth)
{
	if (*cur == NULL)
		return -1;

	(*cur)->bandwidth = bandwidth;

	return 0;
}

int8_t index_stream_list_set_codecs(IndexStreamRecord **cur, const char *codecs)
{
	if (*cur == NULL)
		return -1;

	if((*cur)->codecs != NULL) {
		free ((*cur)->codecs);

		(*cur)->codecs = NULL;
	}

	(*cur)->codecs = (char *) malloc(strlen(codecs) + 1);
	strcpy((*cur)->codecs, codecs);

	return 0;
}

int8_t index_stream_list_set_path(IndexStreamRecord **cur, const char *path)
{
	if (*cur == NULL)
		return -1;

	if ((*cur)->path != NULL) {
		free ((*cur)->path);

		(*cur)->path = NULL;
	}

	(*cur)->path = (char *) malloc(strlen(path) + 1);
	strcpy((*cur)->path, path);

	return 0;
}

int8_t index_stream_list_add(IndexStreamRecord **cur, int8_t program_id, uint64_t bandwidth, const char *codecs, const char *path)
{
	if (*cur == NULL)
		return -1;

	if ((*cur)->prev == NULL && (*cur)->path == NULL) {
		(*cur)->program_id = program_id;
		(*cur)->bandwidth = bandwidth;

		if ((*cur)->codecs != NULL) {
			free ((*cur)->codecs);
			(*cur)->codecs = NULL;
		}

		(*cur)->codecs = (char *) malloc(strlen(codecs) + 1);
		strcpy((*cur)->codecs, codecs);

		(*cur)->path = (char *) malloc(strlen(path) + 1);
		strcpy((*cur)->path, path);
	} else {
		(*cur)->next = (IndexStreamRecord *) malloc(sizeof(IndexStreamRecord));

		if ((*cur)->next == NULL)
			return -2;

		(*cur)->next->program_id = program_id;
		(*cur)->next->bandwidth = bandwidth;

		(*cur)->next->codecs = (char *) malloc(strlen(codecs) + 1);

		if ((*cur)->next->codecs == NULL) {
			free ((*cur)->next);
			return -3;
		}

		strcpy((*cur)->next->codecs, codecs);

		(*cur)->next->path = (char *) malloc(strlen(path) + 1);

		if ((*cur)->next->path == NULL) {
			free ((*cur)->next);
			return -4;
		}

		strcpy((*cur)->next->path, path);

		(*cur)->next->prev = *cur;
		(*cur)->next->next = NULL;
		*cur = (*cur)->next;
		(*cur)->prev->next = *cur;
	}

	return 0;
}

int8_t index_stream_list_destroy(IndexStreamRecord **head)
{
	IndexStreamRecord *tmp = NULL;

	if (*head == NULL)
		return -1;

	while (*head) {
		tmp = *head;
		*head = (*head)->next;
		free (tmp->path);
		free (tmp->codecs);
		free (tmp);
	}

	return 0;
}
