#include "utils.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>

//#include <openjpeg-2.4/openjpeg.h>
//#include <openjpeg-2.4/opj_config.h>
//#include <openjpeg-2.4/opj_stdint.h>

#include <openjpeg-2.5/openjpeg.h>
#include <openjpeg-2.5/opj_config.h>
#include <openjpeg-2.5/opj_stdint.h>

#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <memory.h>
#include "image.h"
#include <math.h>

static OPJ_SIZE_T ReadCallback(void* pBuffer, OPJ_SIZE_T nBytes,
        void *pUserData) {
    fileBuffer* buf = (fileBuffer*) pUserData;
    if (buf->cur >= buf->len) {
        return -1;
    }
    if (buf->cur + nBytes >= buf->len) {
        size_t nToRead = buf->len - buf->cur;
        memcpy(pBuffer, buf->data + buf->cur, nToRead);
        buf->cur = buf->len;
        return nToRead;
    }
    if (nBytes == 0) {
        return -1;
    }
    memcpy(pBuffer, buf->data + buf->cur, nBytes);
    buf->cur += nBytes;
    return nBytes;
}

static OPJ_BOOL SeekCallback(OPJ_OFF_T nBytes, void * pUserData) {
    fileBuffer* buf = (fileBuffer*) pUserData;
    buf->cur = nBytes;
    return OPJ_TRUE;
}

static OPJ_OFF_T SkipCallback(OPJ_OFF_T nBytes, void * pUserData) {
    fileBuffer* buf = (fileBuffer*) pUserData;
    buf->cur += nBytes;
    return nBytes;
}

static void FreeDataCallback(void * pUserData) {
    fileBuffer* buf = (fileBuffer*) pUserData;
    //    free(buf->data);
    //    buf->data = NULL;
    buf->cur = 0;
}


opj_image_t* decode_image(fileBuffer *buff){  
    buff->cur = 0;
    
    opj_image_t* image = NULL;
    opj_codec_t* l_codec = NULL;

    opj_dparameters_t parameters;
    opj_set_default_decoder_parameters(&parameters);
    parameters.decod_format = OPJ_CODEC_JP2;


    opj_stream_t *l_stream = opj_stream_create(1024, OPJ_TRUE);

    opj_stream_set_user_data_length(l_stream, buff->len);
    opj_stream_set_read_function(l_stream, ReadCallback);
    opj_stream_set_seek_function(l_stream, SeekCallback);
    opj_stream_set_skip_function(l_stream, SkipCallback);
    opj_stream_set_user_data(l_stream, buff, FreeDataCallback);

    l_codec = opj_create_decompress(OPJ_CODEC_JP2);

    if (!opj_setup_decoder(l_codec, &parameters)) {
        opj_destroy_codec(l_codec);
        opj_stream_destroy(l_stream);
        opj_image_destroy(image);
        return 0;
    }

    if (!opj_read_header(l_stream, l_codec, &image)) {
        opj_destroy_codec(l_codec);
        opj_stream_destroy(l_stream);
        opj_image_destroy(image);
        return 0;
    }

    if (!(opj_decode(l_codec, l_stream, image) &&
            opj_end_decompress(l_codec, l_stream))) {
        opj_destroy_codec(l_codec);
        opj_stream_destroy(l_stream);
        opj_image_destroy(image);
        return 0;
    }

    opj_destroy_codec(l_codec);
    opj_stream_destroy(l_stream);
//    opj_image_destroy(image);
    return image;
}

void load_image(fileBuffer* buf) {
    FILE *fsrc = fopen(buf->path, "rb");
    fseek(fsrc, 0, SEEK_END);
    int file_length = ftell(fsrc);
    fseek(fsrc, 0, SEEK_SET);
    unsigned char *src = (unsigned char *) malloc(file_length);
    fread(src, 1, file_length, fsrc);
    fclose(fsrc);

    //    fileBuffer* buf = malloc(sizeof (fileBuffer));
    buf->data = src;
    buf->len = file_length;
//    buf->cur = 0;
    buf->img = decode_image(buf);
    //    buf->path = malloc(strlen(path) + 1);
    //    strcpy(buf->path, path);
    //    return buf;
}


int check_image(fileBuffer *buff) {
    opj_image_t* image = decode_image(buff);
    if(image==NULL)
        return 0;
    opj_image_destroy(image);
    return 1;
}

int file_count(dirInfo* df) {
    struct dirent *files;
    int file_count = 0;
    while ((files = readdir(df->dir)) != NULL) {
        if (strcmp(files->d_name, "..") == 0 || strcmp(files->d_name, ".") == 0)
            continue;
        file_count++;
    }
    return file_count;
}

dirInfo* load_dir(const char* dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        printf("Directory cannot be opened!");
        return 0;
    }
    dirInfo* df = (dirInfo*) malloc(sizeof (dirInfo));
    df->dir = dir;
    df->path = malloc(strlen(dir_path) + 1);
    strcpy(df->path, dir_path);
    df->count = file_count(df);

    //
    struct dirent *files;
    fileBuffer* fileList = malloc(sizeof (fileBuffer) * df->count);
    rewinddir(df->dir);
    char file_path[256];
    int index = 0;
    while ((files = readdir(df->dir)) != NULL) {
        if (strcmp(files->d_name, "..") == 0 || strcmp(files->d_name, ".") == 0)
            continue;
        strcpy(file_path, df->path);
        strcat(file_path, files->d_name);
        //        init_image(file_path, &fileList[index]);
        char* path = malloc(strlen(file_path) + 1);
        strcpy(path, file_path);
        fileList[index].path = path;
        load_image(&fileList[index]);
        char* name = malloc(strlen(files->d_name) + 1);
        strcpy(name, files->d_name);
        fileList[index].name = name;
        index++;
    }
    df->files = fileList;
    return df;
}

void free_dirInfo(dirInfo* df) {
    for (int i = 0; i < df->count; i++) {
        free_fileinfo(&df->files[i]);
    }
    closedir(df->dir);
    free(df->path);
    free(df->files);
    free(df);
}



void free_fileinfo(fileBuffer* buf) {
    if (buf->data)
        free(buf->data);
    if (buf->path)
        free(buf->path);
    //    free(buf);
}

double image_psnr(fileBuffer* orig, fileBuffer* rec){
    
    opj_image_t* img_orig = orig->img;
    opj_image_t* img_rec = decode_image(rec);
    
    if(img_rec==NULL)
        return -1;
    
    int32_t *d1 = img_orig->comps[0].data;
    int32_t *d2 = img_rec->comps[0].data;
    
    int32_t w = img_orig->comps[0].w;
    int32_t h = img_rec->comps[0].h;
    
    double E = 0;
    for(int i=0; i<w*h; i++){
        E += pow(d1[i] - d2[i], 2);
    }
    double ME = E / (w*h);
    
    opj_image_destroy(img_rec);
    
    return 10*log10(pow(255,2)/ME);
}

void save_img(void* data, size_t len, char *filename){
    FILE *file = fopen(filename, "w");
    size_t w = fwrite(data, 1, len, file);
    fclose(file);
}
