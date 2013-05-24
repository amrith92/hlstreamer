#ifndef _SNAPPER_H_DEFINED
#define _SNAPPER_H_DEFINED

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#define _SNAPS_CAPS "video/x-raw,format=RGB,width=160,pixel-aspect-ratio=1/1"

/**
 *	For each recognized media file in the directory pointed to by scan_path,
 *	generate a thumbnail and store in the destination provided (PNG format)
 */
extern int8_t generate_snaps_of(const char *scan_path, const char *dest_path);

/**
 *	For a particular media file, generate and save image to dest_path
 */
extern int8_t generate_snap_of(const char *path, const char *dest_path);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SNAPPER_H_DEFINED  */
