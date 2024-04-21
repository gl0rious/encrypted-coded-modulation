
#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <dirent.h>
//#include <openjpeg-2.4/openjpeg.h>
#include <openjpeg-2.5/openjpeg.h>

    
typedef struct {
    unsigned char *data;
    size_t len;
    size_t cur;
    char* path;
    char* name;
    opj_image_t* img;
} fileBuffer;

typedef struct {
    DIR *dir;
    char* path;
    int count;
    fileBuffer* files;
} dirInfo;


dirInfo* load_dir(const char* dir_path);

void load_image(fileBuffer *buf);

int check_image(fileBuffer *buff);

void free_fileinfo(fileBuffer* buf);
void free_dirInfo(dirInfo* df);

//fileBuffer* next_image(dirInfo* df);
        
double image_psnr(fileBuffer* orig, fileBuffer* rec);
void save_img(void* data, size_t len, char *filename);

#ifdef __cplusplus
}
#endif

#endif /* IMAGE_H */

