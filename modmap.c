#include "modmap.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <string.h>

void init_map(enum map_type type, ctxParams* param) {

    float complex psk8_map[] = {0.9238 + 0.3826i, 0.3826 + 0.9238i, -0.3826 + 0.9238i, -0.9238 + 0.3826i, -0.9238 - 0.3826i, -0.3826 - 0.9238i, 0.3826 - 0.9238i, 0.9238 - 0.3826i};
    float complex circ16_map[] = {-2.2304 - 0.9239i, -0.9239 - 2.2304i, 2.2304 - 0.9239i, 0.9239 - 2.2304i, -2.2304 + 0.9239i, -0.9239 + 2.2304i, 2.2304 + 0.9239i, 0.9239 + 2.2304i, -0.9239 - 0.3827i, -0.3827 - 0.9239i, 0.9239 - 0.3827i, 0.3827 - 0.9239i, -0.9239 + 0.3827i, -0.3827 + 0.9239i, 0.9239 + 0.3827i, 0.3827 + 0.9239i};
    float complex circ16_2_map[] = {-1.2071 + 0.5000i, -0.5000 + 1.2071i, -0.5000 + 0.2071i, -0.2071 + 0.5000i, 1.2071 + 0.5000i, 0.5000 + 1.2071i, 0.5000 + 0.2071i, 0.2071 + 0.5000i, -1.2071 - 0.5000i, -0.5000 - 1.2071i, -0.5000 - 0.2071i, -0.2071 - 0.5000i, 1.2071 - 0.5000i, 0.5000 - 1.2071i, 0.5000 - 0.2071i, 0.2071 - 0.5000i};
    float complex qam16_map[] = {-3 - 3i, -3 - 1i, -1 - 3i, -1 - 1i, 3 - 3i, 3 - 1i, 1 - 3i, 1 - 1i, -3 + 3i, -3 + 1i, -1 + 3i, -1 + 1i, 3 + 3i, 3 + 1i, 1 + 3i, 1 + 1i};
    float complex rotated_8_8_map[] = {-2.2304 + 0.9239i, -0.9239 + 2.2304i, -0.9239 + 0.3827i, -0.3827 + 0.9239i, 2.2304 + 0.9239i, 0.9239 + 2.2304i, 0.9239 + 0.3827i, 0.3827 + 0.9239i, -2.2304 - 0.9239i, -0.9239 - 2.2304i, -0.9239 - 0.3827i, -0.3827 - 0.9239i, 2.2304 - 0.9239i, 0.9239 - 2.2304i, 0.9239 - 0.3827i, 0.3827 - 0.9239i};
    float complex rotated_8_8_2_map[] = {-2.2304 + 0.9239i, -0.9239 + 2.2304i, 2.2304 + 0.9239i, 0.9239 + 2.2304i, -2.2304 - 0.9239i, -0.9239 - 2.2304i, 2.2304 - 0.9239i, 0.9239 - 2.2304i, -0.9239 + 0.3827i, -0.3827 + 0.9239i, 0.9239 + 0.3827i, 0.3827 + 0.9239i, -0.9239 - 0.3827i, -0.3827 - 0.9239i, 0.9239 - 0.3827i, 0.3827 - 0.9239i};
    int maplen = 0;
    switch (type) {
        case PSK8:
            maplen = 8;
            strcpy(param->map_name, "PSK8");
//            assert(maplen == LEN(psk8_map));
            for (int i = 0; i < maplen; i++)
                //                map[i] = cexp(2*pi*I*((i+1)/8.0));
                param->map[i] = psk8_map[i];
            break;
        case QAM16:
            maplen = 16;
            strcpy(param->map_name, "QAM16");
//            assert(maplen == LEN(qam16_map));
            for (int i = 0; i < maplen; i++)
                param->map[i] = qam16_map[i];
            break;
        case CIRC16:
            maplen = 16;
            strcpy(param->map_name, "CIRC16");
//            assert(maplen == LEN(circ16_map));
            for (int i = 0; i < maplen; i++)
                param->map[i] = circ16_map[i];
            break;
        case CIRC16_2:
            maplen = 16;
            strcpy(param->map_name, "CIRC16_2");
//            assert(maplen == LEN(circ16_2_map));
            for (int i = 0; i < maplen; i++)
                param->map[i] = circ16_2_map[i];
            break;
        case ROTATED_8_8:
            maplen = 16;
            strcpy(param->map_name, "ROTATED_8_8");
//            assert(maplen == LEN(rotated_8_8_map));
            for (int i = 0; i < maplen; i++)
                param->map[i] = rotated_8_8_map[i];
            break;
        case ROTATED_8_8_2:
            maplen = 16;
            strcpy(param->map_name, "ROTATED_8_8_2");
//            assert(maplen == LEN(rotated_8_8_2_map));
            for (int i = 0; i < maplen; i++)
                param->map[i] = rotated_8_8_2_map[i];
            break;
        default:
            assert(0);
            break;
    }
    float sum = 0;
    for (int i = 0; i < maplen; i++)
        sum += pow(cabs(param->map[i]), 2) / maplen;
    float scale = sqrt(sum);
    for (int i = 0; i < maplen; i++)
        param->map[i] = param->map[i] / scale;
}