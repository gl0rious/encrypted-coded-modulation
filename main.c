#include <stdio.h>
#include <complex.h> 
#include <time.h>
#include "utils.h"
#include "aes.h"
#include "modmap.h"
#include "trellis.h"
#include "ttcm.h"
#include "uttcm.h"
#include "intrlv.h"
#include "contex.h"
#include <math.h>
#include "image.h"
#include <dirent.h>
#include <string.h>
#include <omp.h>
#include <limits.h>
#include <stdlib.h>
#include "ebno.h"

extern int rand_r(unsigned int *__seed) __THROW;

float EbN0list[100];
int EbN0count = 0;

EbN0 *EbN0list2;

time_t start;

fileBuffer cbuf; // = malloc(sizeof(fileBuffer));
u8 cbuf_data[30000];

void print_time() {
    time_t end = time(NULL) - start;
    int hours = end / 60 / 60;
    int mins = end / 60 - (hours * 60);
    int secs = end - (mins * 60) - (hours * 60 * 60);
    printf("time : %i hh %i mm %i ss", hours, mins, secs);
}

ctxParams cntx1;
ctxParams cntx2;
ctxParams cntx3;

//int uintrlv[] = {19, 53, 45, 26, 34, 7, 0, 16, 61, 47, 54, 38, 29, 8, 15, 63, 22, 46, 56, 32, 3, 14, 39, 24, 48, 57, 31, 2, 9, 40, 17, 49, 58, 28, 1, 10, 42, 20, 35, 62, 55, 27, 11, 43, 18, 36, 4, 52, 25, 60, 44, 12, 37, 5, 51, 30, 21, 59, 13, 41, 6, 50, 33, 23};
int uintrlv[] = {15, 56, 48, 7, 29, 63, 22, 39, 14, 46, 55, 3, 31, 23, 62, 16, 44, 52, 8, 37, 27, 0, 59, 18, 47, 10, 36, 25, 2, 60, 17, 53, 43, 34, 9, 1, 61, 24, 51, 41, 32, 11, 4, 58, 19, 49, 40, 33, 26, 5, 12, 57, 50, 42, 35, 28, 20, 6, 13, 54, 45, 38, 30, 21};
int intrlv[] = {24, 53, 2, 39, 38, 25, 50, 7, 14, 63, 28, 49, 0, 35, 62, 23, 40, 13, 18, 1, 52, 45, 6, 61, 30, 31, 42, 17, 16, 3, 56, 41, 4, 55, 26, 29, 46, 19, 36, 9, 60, 43, 10, 59, 20, 33, 48, 21, 32, 11, 58, 47, 8, 57, 22, 37, 44, 27, 34, 15, 54, 5, 12, 51};

void init_context(ctxParams* pttcm, ctxParams* puttcm, ctxParams* puttcm8) {
    int simlen = 1E8;
    int frmlen = 128;
    int iters = 4;
    int imgcount = 1E4;
//        int imgcount = 30;

    // TTCM
    strcpy(pttcm->name, "TTCM");
    pttcm->simlen = simlen;
    pttcm->frmlen = frmlen;
    pttcm->imgcount = imgcount;
    pttcm->iters = iters;
    pttcm->k = 2;
    pttcm->n = pttcm->k + 1;
    pttcm->m = 3;
    u8 coef1[] = {11, 4, 2};
    memcpy(pttcm->coef, coef1, sizeof (coef1));
    pttcm->coeflen = sizeof (coef1);
    init_param_ctx(pttcm);
    memcpy(pttcm->intrlv, intrlv, sizeof (intrlv));
    init_map(PSK8, pttcm);
    pttcm->encode = encode_ttcm;
    pttcm->decode = decode_ttcm;

    // UTTCM
    strcpy(puttcm->name, "UTTCM");
    puttcm->simlen = simlen;
    puttcm->frmlen = frmlen;
    puttcm->imgcount = imgcount;
    puttcm->iters = iters;
    puttcm->k = 2;
    puttcm->n = puttcm->k + 2;
    puttcm->m = 4;
    u8 coef2[] = {23, 13, 5};
    memcpy(puttcm->coef, coef2, sizeof (coef2));
    puttcm->coeflen = sizeof (coef2);
    init_param_ctx(puttcm);
    memcpy(puttcm->intrlv, uintrlv, sizeof (uintrlv));
    init_map(QAM16, puttcm);
    puttcm->encode = encode_uttcm;
    puttcm->decode = decode_uttcm;

    // UTTCM 8
    strcpy(puttcm8->name, "UTTCM");
    puttcm8->simlen = simlen;
    puttcm8->frmlen = frmlen;
    puttcm8->imgcount = imgcount;
    puttcm8->iters = iters;
    puttcm8->k = 2;
    puttcm8->n = puttcm8->k + 2;
    puttcm8->m = 4;
    memcpy(puttcm8->coef, coef2, sizeof (coef2));
    puttcm8->coeflen = sizeof (coef2);
    init_param_ctx(puttcm8);
    memcpy(puttcm8->intrlv, uintrlv, sizeof (uintrlv));
    init_map(ROTATED_8_8, puttcm8);
    puttcm8->encode = encode_uttcm;
    puttcm8->decode = decode_uttcm;
}

int bit_frm_sim() {
    init_context(&cntx1, &cntx2, &cntx3);
    ctxParams * params[] = {&cntx1, &cntx2, &cntx3};

    for (int ctx = 0; ctx < 3; ctx++) {
        ctxParams * param = params[ctx];
        printf("%s (%s) started ...\n", param->name, param->map_name);
        printf("\tsimlen = %1.1e\n", (float) param->simlen);
        printf("\tfrmcount = %i\n", param->frmcount);

        EbN0count = init_EbN0(3.5, 1, 5.5, EbN0list);
        for (int ei = 0; ei < EbN0count; ei++) {
            ctxSim _sim;
            ctxSim* sim = &_sim;
            init_sim_ctx(param, sim);

            sim->data_seed = 975;
            sim->noise_seed = 531;

            ctxAES ctxAes;
            aes_init(&ctxAes);

            float EbN0 = EbN0list[ei];
            int berr = 0;
            int ferr = 0;
            int cberr = 0;
            int cferr = 0;
            for (int it = 0; it < param->iters; it++)
                sim->bers[it] = 0;

            for (int i = 0; i < param->frmcount; i++) {
                rand_bytes(param, sim);
                aes_encrypt_cbc(&ctxAes, sim->data, sim->data_c, param->datalen);
                unpack_bytes(sim->data_c, sim->x, param->k, param->datalen);

                param->encode(param, sim);
                param->sigma = EbN0_to_sigma(EbN0, param->k);
                add_noise(param, sim);
                param->decode(param, sim);

                pack_bytes(sim->data_d, sim->xh, param->k, param->datalen);

                berr += biterr(sim->data_c, sim->data_d, param->datalen);
                ferr += frmerr(sim->data_c, sim->data_d, param->datalen);

                aes_decrypt_cbc(&ctxAes, sim->data_d, sim->datah, param->datalen);
                cberr += biterr(sim->data, sim->datah, param->datalen);
                cferr += frmerr(sim->data, sim->datah, param->datalen);
            }

            printf(" EbN0 = %4.2f | ", EbN0);

            printf("ber = [%8i  %8.2f]  ", berr, log10((float) berr /
                    (param->frmcount * param->frmlen)));
            printf("fer = [%8i  %8.2f]  ", ferr, log10((float) ferr /
                    param->frmcount));
            printf("cber = [%8i  %8.2f]  ", cberr, log10((float) cberr /
                    (param->frmcount * param->frmlen)));
            printf("cfer = [%8i  %8.2f]  \n", cferr, log10((float) cferr /
                    param->frmcount));
            if (ferr == 0)
                break;
        }
    }
    return (EXIT_SUCCESS);
}

int process_image(ctxParams* param, ctxSim* sim, fileBuffer* buf, double EbN0) {

    ctxAES ctxAes;
    aes_init(&ctxAes);

    fileBuffer cbuf; // = malloc(sizeof(fileBuffer));
    u8 cbuf_data[30000];
    if (buf->len >= 30000) {
        printf("image file size is bigger than cbuf_data\n");
        exit(EXIT_FAILURE);
    }
    cbuf.data = cbuf_data; //malloc(sizeof (u8) * buf->len);
    cbuf.cur = 0;
    cbuf.len = buf->len;
    cbuf.path = 0;
    int blk_count = buf->len / param->datalen;
    for (int b = 0; b <= blk_count; b++) {
        int blkoffset = b * param->datalen;
        int blklen = MIN(buf->len - blkoffset, param->datalen);

        if (param->use_encrypt) {
            aes_encrypt_cbc(&ctxAes, buf->data + blkoffset, sim->data_c, param->datalen);
            unpack_bytes(sim->data_c, sim->x, param->k, param->datalen);
        } else {
            //memset(sim->data, 0, param->datalen);
            memcpy(sim->data, buf->data + blkoffset, blklen);
            unpack_bytes(sim->data, sim->x, param->k, param->datalen);
        }
        param->encode(param, sim);
        param->sigma = EbN0_to_sigma(EbN0, param->k);
        add_noise(param, sim);
        param->decode(param, sim);

        pack_bytes(sim->data_d, sim->xh, param->k, param->datalen);
        if (param->use_encrypt) {
            aes_decrypt_cbc(&ctxAes, sim->data_d, cbuf.data + blkoffset, param->datalen);
        } else {
            memcpy(cbuf.data + blkoffset, sim->data_d, blklen);
        }
    }
    int valid = check_image(&cbuf);
    return valid;
}

void image_sim(int idx, int encrypt) {
    //    printf("i = %i\n", idx);
    init_context(&cntx1, &cntx2, &cntx3);

    dirInfo* df = load_dir("/home/majid/Dropbox/j2k/jp2/");

    assert(idx == 1 || idx == 2 || idx == 3);
    ctxParams* param; // = idx == 1 ? &cntx1 : &cntx2;
    switch (idx) {
        case 1: param = &cntx1;
            break;
        case 2: param = &cntx2;
            break;
        case 3: param = &cntx3;
            break;
    }
    assert(encrypt == 0 || encrypt == 1);
    param->use_encrypt = encrypt;

    ctxSim sim;
    init_sim_ctx(param, &sim);

    EbN0count = init_EbN0(3.5, 1, 12, EbN0list);


    printf("%s (%s) started ... %s encryption\n",
            param->name, param->map_name,
            param->use_encrypt ? "with" : "without");
    for (int ei = 0; ei < EbN0count; ei++) {
        sim.data_seed = 975;
        sim.noise_seed = 531;
        float EbN0 = EbN0list[ei];

        int imgErr = 0;
        for (int i = 0; i < param->imgcount; i++) {
            int idx = rand_r(&sim.data_seed) % df->count;
            fileBuffer* buf = &df->files[idx];
            int valid = process_image(param, &sim, buf, EbN0);
            printf("\r image %i of %i => ", i + 1, param->imgcount);
            print_time();
            fflush(stdout);
            if (valid == 0)
                imgErr++;
        }
        printf("\rEbN0(%i) = %4.2f;    imgErr = %12i                                        \n",
                ei + 1, EbN0, imgErr);
        if (imgErr == 0)
            break;
    }

    free_dirInfo(df);
}

void image_sim2(int idx, int encrypt) {
    //    printf("i = %i\n", idx);
    init_context(&cntx1, &cntx2, &cntx3);

    dirInfo* df = load_dir("/home/majid/Dropbox/j2k/jp2/");

    assert(idx == 1 || idx == 2 || idx == 3);
    ctxParams* param; // = idx == 1 ? &cntx1 : &cntx2;
    switch (idx) {
        case 1: param = &cntx1;
            break;
        case 2: param = &cntx2;
            break;
        case 3: param = &cntx3;
            break;
    }

    assert(encrypt == 0 || encrypt == 1);
    param->use_encrypt = encrypt;

    ctxSim sim;
    init_sim_ctx(param, &sim);

    EbN0list2 = init_EbN02(3, 12);


    printf("%s (%s) started ... %s encryption\n",
            param->name, param->map_name,
            param->use_encrypt ? "with" : "without");

    EbN0 *ebnoIter = EbN0list2;

    while (ebnoIter != NULL) {
        if (ebnoIter->err == -1) {
            sim.data_seed = 975;
            sim.noise_seed = 531;

            ebnoIter->err = 0;
            for (int i = 0; i < param->imgcount; i++) {
                int idx = rand_r(&sim.data_seed) % df->count;
                fileBuffer* buf = &df->files[idx];
                int valid = process_image(param, &sim, buf, ebnoIter->ebno);
                printf("\r EbN0 = %4.2f ... image %i of %i => ", ebnoIter->ebno, i + 1, param->imgcount);
                print_time();
                fflush(stdout);
                if (valid == 0)
                    ebnoIter->err++;
            }
            printf("\rEbN0 = %4.2f;    imgErr = %12i                                        \n",
                    ebnoIter->ebno, ebnoIter->err);
        }
        if (ebnoIter->prev != NULL && abs(ebnoIter->err - ebnoIter->prev->err) > 500 /*&& fabs(ebnoIter->ebno - ebnoIter->prev->ebno) >= 0.2*/) {
            ebnoIter = insert_before(ebnoIter);
        } else
            ebnoIter = ebnoIter->next;
        if (ebnoIter != NULL && ebnoIter->err == 0)
            break;
    }
    printf("sorted :\n");
    ebnoIter = EbN0list2;
    while (ebnoIter != NULL) {
        printf("\rEbN0 = %4.2f;    imgErr = %12i                                        \n",
                ebnoIter->ebno, ebnoIter->err);
        ebnoIter = ebnoIter->next;
    }

    free_dirInfo(df);
}

double process_image_psnr(ctxParams* param, ctxSim* sim, fileBuffer* buf, double EbN0) {

    ctxAES ctxAes;
    aes_init(&ctxAes);


    if (buf->len >= 30000) {
        printf("image file size is bigger than cbuf_data\n");
        exit(EXIT_FAILURE);
    }
    cbuf.data = cbuf_data; //malloc(sizeof (u8) * buf->len);
    cbuf.cur = 0;
    cbuf.len = buf->len;
    cbuf.path = 0;
    int blk_count = buf->len / param->datalen;
    for (int b = 0; b <= blk_count; b++) {
        int blkoffset = b * param->datalen;
        int blklen = MIN(buf->len - blkoffset, param->datalen);

        if (param->use_encrypt) {
            aes_encrypt_cbc(&ctxAes, buf->data + blkoffset, sim->data_c, param->datalen);
            unpack_bytes(sim->data_c, sim->x, param->k, param->datalen);
        } else {
            //memset(sim->data, 0, param->datalen);
            memcpy(sim->data, buf->data + blkoffset, blklen);
            unpack_bytes(sim->data, sim->x, param->k, param->datalen);
        }
        param->encode(param, sim);
        param->sigma = EbN0_to_sigma(EbN0, param->k);
        add_noise(param, sim);
        param->decode(param, sim);

        pack_bytes(sim->data_d, sim->xh, param->k, param->datalen);
        if (param->use_encrypt) {
            aes_decrypt_cbc(&ctxAes, sim->data_d, cbuf.data + blkoffset, param->datalen);
        } else {
            memcpy(cbuf.data + blkoffset, sim->data_d, blklen);
        }
    }
    return image_psnr(buf, &cbuf);
}

void image_psnr_search(int idx, int encrypt) {
    //    printf("i = %i\n", idx);
    init_context(&cntx1, &cntx2, &cntx3);

    dirInfo* df = load_dir("/home/majid/Dropbox/j2k/jp2/");

    assert(idx == 1 || idx == 2);
    ctxParams* param = idx == 1 ? &cntx1 : &cntx2;

    assert(encrypt == 0 || encrypt == 1);
    param->use_encrypt = encrypt;

    ctxSim sim;
    init_sim_ctx(param, &sim);

    EbN0count = init_EbN0(4, 0.1, 50, EbN0list);

    printf("%s (%s) started ... %s encryption\n",
            param->name, param->map_name,
            param->use_encrypt ? "with" : "without");

    int psnri[100] = {0};

    for (int ei = 0; ei < EbN0count; ei++) {
        sim.data_seed = 975;
        sim.noise_seed = 531;
        float EbN0 = EbN0list[ei];
        printf("EbN0 = %4.2f\n", EbN0);
        for (int i = 0; i < param->imgcount; i++) {
            int idx = rand_r(&sim.data_seed) % df->count;
            fileBuffer* buf = &df->files[idx];
            double psnr = process_image_psnr(param, &sim, buf, EbN0);
            // corrupted received image (-1) or image perfectly received (inf)
            if (psnr == -1 || psnr > 100)
                continue;
            printf("psnr = %4.2f\n", psnr);

            int pi = (int) (psnr);
            if (psnri[pi] == 0) {
                psnri[pi] = 1;
                char filename[512];
                snprintf(filename, sizeof (filename),
                        "/home/majid/Desktop/output/psnr_%4.2f-enbo_%4.2f-%s.jp2",
                        psnr, EbN0, buf->name);
                save_img(cbuf.data, cbuf.len, filename);
                break;
            }
        }
    }

    free_dirInfo(df);
}

void image_psnr_search2(int idx, int encrypt) {
    //    printf("i = %i\n", idx);
    init_context(&cntx1, &cntx2, &cntx3);

    dirInfo* df = load_dir("/home/majid/Dropbox/j2k/jp2/");

    assert(idx == 1 || idx == 2);
    ctxParams* param = idx == 1 ? &cntx1 : &cntx2;

    assert(encrypt == 0 || encrypt == 1);
    param->use_encrypt = encrypt;

    ctxSim sim;
    init_sim_ctx(param, &sim);

    EbN0count = init_EbN0(3, 0.5, 5, EbN0list);


    printf("%s (%s) started ... %s encryption\n",
            param->name, param->map_name,
            param->use_encrypt ? "with" : "without");

    for (int ei = 0; ei < EbN0count; ei++) {
        sim.data_seed = 975;
        sim.noise_seed = 531;
        float EbN0 = EbN0list[ei];
        printf("EbN0 = %4.2f\n", EbN0);
        double total_psnr = 0;
        double min_psnr = INFINITY;
        double max_psnr = -INFINITY;
        int psnr_imgcount = 0;
        for (int i = 0; i < param->imgcount; i++) {
            int idx = rand_r(&sim.data_seed) % df->count;
            fileBuffer* buf = &df->files[idx];
            double psnr = process_image_psnr(param, &sim, buf, EbN0);
            // corrupted received image (-1) or image perfectly received (inf)
            if (psnr == -1 || psnr >= INFINITY)
                continue;
            total_psnr += psnr;
            psnr_imgcount++;
            if (psnr > max_psnr)
                max_psnr = psnr;
            if (psnr < min_psnr)
                min_psnr = psnr;
            printf("\r psnr => image count = %4i/%4i, min = %4.2f, max = %4.2f, avg = %4.2f",
                    psnr_imgcount, i, min_psnr, max_psnr, total_psnr / psnr_imgcount);
            fflush(stdout);
        }
        printf("\nmin psnr = %4.2f\n", min_psnr);
        printf("max psnr = %4.2f\n", max_psnr);

        double avg_psnr = total_psnr / psnr_imgcount;
        printf("avg_psnr = %4.2f\n", avg_psnr);
        if (psnr_imgcount == 0)
            continue;

        double closest_psnr = -INFINITY;

        fileBuffer* closest_orig_image;
        u8 temp_buf[30000];
        size_t temp_buf_len = 0;
        for (int i = 0; i < param->imgcount; i++) {
            int idx = rand_r(&sim.data_seed) % df->count;
            fileBuffer* buf = &df->files[idx];
            double psnr = process_image_psnr(param, &sim, buf, EbN0);
            // corrupted received image (-1) or image perfectly received (inf)
            if (psnr == -1 || psnr == INFINITY)
                continue;
            if (fabs(avg_psnr - closest_psnr) > fabs(avg_psnr - psnr)) {
                closest_psnr = psnr;
                closest_orig_image = buf;
                // copy received image data
                memcpy(temp_buf, cbuf.data, cbuf.len);
                temp_buf_len = cbuf.len;
            }
        }
        if (closest_psnr > 0 && closest_orig_image != NULL) {
            char filename[512];
            // save original
            snprintf(filename, sizeof (filename),
                    "/home/majid/Desktop/output/%s_%s%s_enbo_%4.2f-psnr_%4.2f_avg_%4.2f_max_%4.2f_min_%4.2f-orig_%s.jp2",
                    param->name, param->map_name, param->use_encrypt ? "_encrypt" : "", EbN0, closest_psnr, avg_psnr,
                    max_psnr, min_psnr, closest_orig_image->name);
            save_img(closest_orig_image->data, closest_orig_image->len, filename);

            // save received
            snprintf(filename, sizeof (filename),
                    "/home/majid/Desktop/output/%s_%s%s_enbo_%4.2f-psnr_%4.2f_avg_%4.2f_max_%4.2f_min_%4.2f-recv_%s.jp2",
                    param->name, param->map_name, param->use_encrypt ? "_encrypt" : "", EbN0, closest_psnr, avg_psnr,
                    max_psnr, min_psnr, closest_orig_image->name);
            save_img(temp_buf, temp_buf_len, filename);
        }
    }

    free_dirInfo(df);
}

int main(int argc, char** argv) {
    start = time(NULL);

    int i = atoi(argv[1]);
    int j = atoi(argv[2]);
    //        image_sim(i, j);
    //    image_sim2(1, 0);
    //        image_sim(1, 1);
    //        image_sim(2, 0);
    //        image_sim(2, 1);

    //            image_sim(3, 0);
    //            image_sim(3, 1);

    //            bit_frm_sim();

    image_psnr_search2(i, j);
    print_time();
    printf("\n");
}
