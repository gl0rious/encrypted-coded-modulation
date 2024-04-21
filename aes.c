#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "aes.h"
#include <math.h>

const u32 m1 = 0x80808080;
const u32 m2 = 0x7f7f7f7f;
const u32 m3 = 0x0000001b;
const u32 m4 = 0xC0C0C0C0;
const u32 m5 = 0x3f3f3f3f;

const u8 S[256] = {
    (u8) 99, (u8) 124, (u8) 119, (u8) 123, (u8) 242, (u8) 107, (u8) 111, (u8) 197,
    (u8) 48, (u8) 1, (u8) 103, (u8) 43, (u8) 254, (u8) 215, (u8) 171, (u8) 118,
    (u8) 202, (u8) 130, (u8) 201, (u8) 125, (u8) 250, (u8) 89, (u8) 71, (u8) 240,
    (u8) 173, (u8) 212, (u8) 162, (u8) 175, (u8) 156, (u8) 164, (u8) 114, (u8) 192,
    (u8) 183, (u8) 253, (u8) 147, (u8) 38, (u8) 54, (u8) 63, (u8) 247, (u8) 204,
    (u8) 52, (u8) 165, (u8) 229, (u8) 241, (u8) 113, (u8) 216, (u8) 49, (u8) 21,
    (u8) 4, (u8) 199, (u8) 35, (u8) 195, (u8) 24, (u8) 150, (u8) 5, (u8) 154,
    (u8) 7, (u8) 18, (u8) 128, (u8) 226, (u8) 235, (u8) 39, (u8) 178, (u8) 117,
    (u8) 9, (u8) 131, (u8) 44, (u8) 26, (u8) 27, (u8) 110, (u8) 90, (u8) 160,
    (u8) 82, (u8) 59, (u8) 214, (u8) 179, (u8) 41, (u8) 227, (u8) 47, (u8) 132,
    (u8) 83, (u8) 209, (u8) 0, (u8) 237, (u8) 32, (u8) 252, (u8) 177, (u8) 91,
    (u8) 106, (u8) 203, (u8) 190, (u8) 57, (u8) 74, (u8) 76, (u8) 88, (u8) 207,
    (u8) 208, (u8) 239, (u8) 170, (u8) 251, (u8) 67, (u8) 77, (u8) 51, (u8) 133,
    (u8) 69, (u8) 249, (u8) 2, (u8) 127, (u8) 80, (u8) 60, (u8) 159, (u8) 168,
    (u8) 81, (u8) 163, (u8) 64, (u8) 143, (u8) 146, (u8) 157, (u8) 56, (u8) 245,
    (u8) 188, (u8) 182, (u8) 218, (u8) 33, (u8) 16, (u8) 255, (u8) 243, (u8) 210,
    (u8) 205, (u8) 12, (u8) 19, (u8) 236, (u8) 95, (u8) 151, (u8) 68, (u8) 23,
    (u8) 196, (u8) 167, (u8) 126, (u8) 61, (u8) 100, (u8) 93, (u8) 25, (u8) 115,
    (u8) 96, (u8) 129, (u8) 79, (u8) 220, (u8) 34, (u8) 42, (u8) 144, (u8) 136,
    (u8) 70, (u8) 238, (u8) 184, (u8) 20, (u8) 222, (u8) 94, (u8) 11, (u8) 219,
    (u8) 224, (u8) 50, (u8) 58, (u8) 10, (u8) 73, (u8) 6, (u8) 36, (u8) 92,
    (u8) 194, (u8) 211, (u8) 172, (u8) 98, (u8) 145, (u8) 149, (u8) 228, (u8) 121,
    (u8) 231, (u8) 200, (u8) 55, (u8) 109, (u8) 141, (u8) 213, (u8) 78, (u8) 169,
    (u8) 108, (u8) 86, (u8) 244, (u8) 234, (u8) 101, (u8) 122, (u8) 174, (u8) 8,
    (u8) 186, (u8) 120, (u8) 37, (u8) 46, (u8) 28, (u8) 166, (u8) 180, (u8) 198,
    (u8) 232, (u8) 221, (u8) 116, (u8) 31, (u8) 75, (u8) 189, (u8) 139, (u8) 138,
    (u8) 112, (u8) 62, (u8) 181, (u8) 102, (u8) 72, (u8) 3, (u8) 246, (u8) 14,
    (u8) 97, (u8) 53, (u8) 87, (u8) 185, (u8) 134, (u8) 193, (u8) 29, (u8) 158,
    (u8) 225, (u8) 248, (u8) 152, (u8) 17, (u8) 105, (u8) 217, (u8) 142, (u8) 148,
    (u8) 155, (u8) 30, (u8) 135, (u8) 233, (u8) 206, (u8) 85, (u8) 40, (u8) 223,
    (u8) 140, (u8) 161, (u8) 137, (u8) 13, (u8) 191, (u8) 230, (u8) 66, (u8) 104,
    (u8) 65, (u8) 153, (u8) 45, (u8) 15, (u8) 176, (u8) 84, (u8) 187, (u8) 22,
};

// The inverse S-box
const u8 Si[256] = {
    (u8) 82, (u8) 9, (u8) 106, (u8) 213, (u8) 48, (u8) 54, (u8) 165, (u8) 56,
    (u8) 191, (u8) 64, (u8) 163, (u8) 158, (u8) 129, (u8) 243, (u8) 215, (u8) 251,
    (u8) 124, (u8) 227, (u8) 57, (u8) 130, (u8) 155, (u8) 47, (u8) 255, (u8) 135,
    (u8) 52, (u8) 142, (u8) 67, (u8) 68, (u8) 196, (u8) 222, (u8) 233, (u8) 203,
    (u8) 84, (u8) 123, (u8) 148, (u8) 50, (u8) 166, (u8) 194, (u8) 35, (u8) 61,
    (u8) 238, (u8) 76, (u8) 149, (u8) 11, (u8) 66, (u8) 250, (u8) 195, (u8) 78,
    (u8) 8, (u8) 46, (u8) 161, (u8) 102, (u8) 40, (u8) 217, (u8) 36, (u8) 178,
    (u8) 118, (u8) 91, (u8) 162, (u8) 73, (u8) 109, (u8) 139, (u8) 209, (u8) 37,
    (u8) 114, (u8) 248, (u8) 246, (u8) 100, (u8) 134, (u8) 104, (u8) 152, (u8) 22,
    (u8) 212, (u8) 164, (u8) 92, (u8) 204, (u8) 93, (u8) 101, (u8) 182, (u8) 146,
    (u8) 108, (u8) 112, (u8) 72, (u8) 80, (u8) 253, (u8) 237, (u8) 185, (u8) 218,
    (u8) 94, (u8) 21, (u8) 70, (u8) 87, (u8) 167, (u8) 141, (u8) 157, (u8) 132,
    (u8) 144, (u8) 216, (u8) 171, (u8) 0, (u8) 140, (u8) 188, (u8) 211, (u8) 10,
    (u8) 247, (u8) 228, (u8) 88, (u8) 5, (u8) 184, (u8) 179, (u8) 69, (u8) 6,
    (u8) 208, (u8) 44, (u8) 30, (u8) 143, (u8) 202, (u8) 63, (u8) 15, (u8) 2,
    (u8) 193, (u8) 175, (u8) 189, (u8) 3, (u8) 1, (u8) 19, (u8) 138, (u8) 107,
    (u8) 58, (u8) 145, (u8) 17, (u8) 65, (u8) 79, (u8) 103, (u8) 220, (u8) 234,
    (u8) 151, (u8) 242, (u8) 207, (u8) 206, (u8) 240, (u8) 180, (u8) 230, (u8) 115,
    (u8) 150, (u8) 172, (u8) 116, (u8) 34, (u8) 231, (u8) 173, (u8) 53, (u8) 133,
    (u8) 226, (u8) 249, (u8) 55, (u8) 232, (u8) 28, (u8) 117, (u8) 223, (u8) 110,
    (u8) 71, (u8) 241, (u8) 26, (u8) 113, (u8) 29, (u8) 41, (u8) 197, (u8) 137,
    (u8) 111, (u8) 183, (u8) 98, (u8) 14, (u8) 170, (u8) 24, (u8) 190, (u8) 27,
    (u8) 252, (u8) 86, (u8) 62, (u8) 75, (u8) 198, (u8) 210, (u8) 121, (u8) 32,
    (u8) 154, (u8) 219, (u8) 192, (u8) 254, (u8) 120, (u8) 205, (u8) 90, (u8) 244,
    (u8) 31, (u8) 221, (u8) 168, (u8) 51, (u8) 136, (u8) 7, (u8) 199, (u8) 49,
    (u8) 177, (u8) 18, (u8) 16, (u8) 89, (u8) 39, (u8) 128, (u8) 236, (u8) 95,
    (u8) 96, (u8) 81, (u8) 127, (u8) 169, (u8) 25, (u8) 181, (u8) 74, (u8) 13,
    (u8) 45, (u8) 229, (u8) 122, (u8) 159, (u8) 147, (u8) 201, (u8) 156, (u8) 239,
    (u8) 160, (u8) 224, (u8) 59, (u8) 77, (u8) 174, (u8) 42, (u8) 245, (u8) 176,
    (u8) 200, (u8) 235, (u8) 187, (u8) 60, (u8) 131, (u8) 83, (u8) 153, (u8) 97,
    (u8) 23, (u8) 43, (u8) 4, (u8) 126, (u8) 186, (u8) 119, (u8) 214, (u8) 38,
    (u8) 225, (u8) 105, (u8) 20, (u8) 99, (u8) 85, (u8) 33, (u8) 12, (u8) 125,
};

// vector used in calculating key schedule (powers of x in GF(256))
u32 rcon[30] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91
};

// precomputation tables of calculations for rounds
u32 T0[256] = {
    0xa56363c6, 0x847c7cf8, 0x997777ee, 0x8d7b7bf6, 0x0df2f2ff,
    0xbd6b6bd6, 0xb16f6fde, 0x54c5c591, 0x50303060, 0x03010102,
    0xa96767ce, 0x7d2b2b56, 0x19fefee7, 0x62d7d7b5, 0xe6abab4d,
    0x9a7676ec, 0x45caca8f, 0x9d82821f, 0x40c9c989, 0x877d7dfa,
    0x15fafaef, 0xeb5959b2, 0xc947478e, 0x0bf0f0fb, 0xecadad41,
    0x67d4d4b3, 0xfda2a25f, 0xeaafaf45, 0xbf9c9c23, 0xf7a4a453,
    0x967272e4, 0x5bc0c09b, 0xc2b7b775, 0x1cfdfde1, 0xae93933d,
    0x6a26264c, 0x5a36366c, 0x413f3f7e, 0x02f7f7f5, 0x4fcccc83,
    0x5c343468, 0xf4a5a551, 0x34e5e5d1, 0x08f1f1f9, 0x937171e2,
    0x73d8d8ab, 0x53313162, 0x3f15152a, 0x0c040408, 0x52c7c795,
    0x65232346, 0x5ec3c39d, 0x28181830, 0xa1969637, 0x0f05050a,
    0xb59a9a2f, 0x0907070e, 0x36121224, 0x9b80801b, 0x3de2e2df,
    0x26ebebcd, 0x6927274e, 0xcdb2b27f, 0x9f7575ea, 0x1b090912,
    0x9e83831d, 0x742c2c58, 0x2e1a1a34, 0x2d1b1b36, 0xb26e6edc,
    0xee5a5ab4, 0xfba0a05b, 0xf65252a4, 0x4d3b3b76, 0x61d6d6b7,
    0xceb3b37d, 0x7b292952, 0x3ee3e3dd, 0x712f2f5e, 0x97848413,
    0xf55353a6, 0x68d1d1b9, 0x00000000, 0x2cededc1, 0x60202040,
    0x1ffcfce3, 0xc8b1b179, 0xed5b5bb6, 0xbe6a6ad4, 0x46cbcb8d,
    0xd9bebe67, 0x4b393972, 0xde4a4a94, 0xd44c4c98, 0xe85858b0,
    0x4acfcf85, 0x6bd0d0bb, 0x2aefefc5, 0xe5aaaa4f, 0x16fbfbed,
    0xc5434386, 0xd74d4d9a, 0x55333366, 0x94858511, 0xcf45458a,
    0x10f9f9e9, 0x06020204, 0x817f7ffe, 0xf05050a0, 0x443c3c78,
    0xba9f9f25, 0xe3a8a84b, 0xf35151a2, 0xfea3a35d, 0xc0404080,
    0x8a8f8f05, 0xad92923f, 0xbc9d9d21, 0x48383870, 0x04f5f5f1,
    0xdfbcbc63, 0xc1b6b677, 0x75dadaaf, 0x63212142, 0x30101020,
    0x1affffe5, 0x0ef3f3fd, 0x6dd2d2bf, 0x4ccdcd81, 0x140c0c18,
    0x35131326, 0x2fececc3, 0xe15f5fbe, 0xa2979735, 0xcc444488,
    0x3917172e, 0x57c4c493, 0xf2a7a755, 0x827e7efc, 0x473d3d7a,
    0xac6464c8, 0xe75d5dba, 0x2b191932, 0x957373e6, 0xa06060c0,
    0x98818119, 0xd14f4f9e, 0x7fdcdca3, 0x66222244, 0x7e2a2a54,
    0xab90903b, 0x8388880b, 0xca46468c, 0x29eeeec7, 0xd3b8b86b,
    0x3c141428, 0x79dedea7, 0xe25e5ebc, 0x1d0b0b16, 0x76dbdbad,
    0x3be0e0db, 0x56323264, 0x4e3a3a74, 0x1e0a0a14, 0xdb494992,
    0x0a06060c, 0x6c242448, 0xe45c5cb8, 0x5dc2c29f, 0x6ed3d3bd,
    0xefacac43, 0xa66262c4, 0xa8919139, 0xa4959531, 0x37e4e4d3,
    0x8b7979f2, 0x32e7e7d5, 0x43c8c88b, 0x5937376e, 0xb76d6dda,
    0x8c8d8d01, 0x64d5d5b1, 0xd24e4e9c, 0xe0a9a949, 0xb46c6cd8,
    0xfa5656ac, 0x07f4f4f3, 0x25eaeacf, 0xaf6565ca, 0x8e7a7af4,
    0xe9aeae47, 0x18080810, 0xd5baba6f, 0x887878f0, 0x6f25254a,
    0x722e2e5c, 0x241c1c38, 0xf1a6a657, 0xc7b4b473, 0x51c6c697,
    0x23e8e8cb, 0x7cdddda1, 0x9c7474e8, 0x211f1f3e, 0xdd4b4b96,
    0xdcbdbd61, 0x868b8b0d, 0x858a8a0f, 0x907070e0, 0x423e3e7c,
    0xc4b5b571, 0xaa6666cc, 0xd8484890, 0x05030306, 0x01f6f6f7,
    0x120e0e1c, 0xa36161c2, 0x5f35356a, 0xf95757ae, 0xd0b9b969,
    0x91868617, 0x58c1c199, 0x271d1d3a, 0xb99e9e27, 0x38e1e1d9,
    0x13f8f8eb, 0xb398982b, 0x33111122, 0xbb6969d2, 0x70d9d9a9,
    0x898e8e07, 0xa7949433, 0xb69b9b2d, 0x221e1e3c, 0x92878715,
    0x20e9e9c9, 0x49cece87, 0xff5555aa, 0x78282850, 0x7adfdfa5,
    0x8f8c8c03, 0xf8a1a159, 0x80898909, 0x170d0d1a, 0xdabfbf65,
    0x31e6e6d7, 0xc6424284, 0xb86868d0, 0xc3414182, 0xb0999929,
    0x772d2d5a, 0x110f0f1e, 0xcbb0b07b, 0xfc5454a8, 0xd6bbbb6d,
    0x3a16162c
};

u32 Tinv0[256] = {
    0x50a7f451, 0x5365417e, 0xc3a4171a, 0x965e273a, 0xcb6bab3b,
    0xf1459d1f, 0xab58faac, 0x9303e34b, 0x55fa3020, 0xf66d76ad,
    0x9176cc88, 0x254c02f5, 0xfcd7e54f, 0xd7cb2ac5, 0x80443526,
    0x8fa362b5, 0x495ab1de, 0x671bba25, 0x980eea45, 0xe1c0fe5d,
    0x02752fc3, 0x12f04c81, 0xa397468d, 0xc6f9d36b, 0xe75f8f03,
    0x959c9215, 0xeb7a6dbf, 0xda595295, 0x2d83bed4, 0xd3217458,
    0x2969e049, 0x44c8c98e, 0x6a89c275, 0x78798ef4, 0x6b3e5899,
    0xdd71b927, 0xb64fe1be, 0x17ad88f0, 0x66ac20c9, 0xb43ace7d,
    0x184adf63, 0x82311ae5, 0x60335197, 0x457f5362, 0xe07764b1,
    0x84ae6bbb, 0x1ca081fe, 0x942b08f9, 0x58684870, 0x19fd458f,
    0x876cde94, 0xb7f87b52, 0x23d373ab, 0xe2024b72, 0x578f1fe3,
    0x2aab5566, 0x0728ebb2, 0x03c2b52f, 0x9a7bc586, 0xa50837d3,
    0xf2872830, 0xb2a5bf23, 0xba6a0302, 0x5c8216ed, 0x2b1ccf8a,
    0x92b479a7, 0xf0f207f3, 0xa1e2694e, 0xcdf4da65, 0xd5be0506,
    0x1f6234d1, 0x8afea6c4, 0x9d532e34, 0xa055f3a2, 0x32e18a05,
    0x75ebf6a4, 0x39ec830b, 0xaaef6040, 0x069f715e, 0x51106ebd,
    0xf98a213e, 0x3d06dd96, 0xae053edd, 0x46bde64d, 0xb58d5491,
    0x055dc471, 0x6fd40604, 0xff155060, 0x24fb9819, 0x97e9bdd6,
    0xcc434089, 0x779ed967, 0xbd42e8b0, 0x888b8907, 0x385b19e7,
    0xdbeec879, 0x470a7ca1, 0xe90f427c, 0xc91e84f8, 0x00000000,
    0x83868009, 0x48ed2b32, 0xac70111e, 0x4e725a6c, 0xfbff0efd,
    0x5638850f, 0x1ed5ae3d, 0x27392d36, 0x64d90f0a, 0x21a65c68,
    0xd1545b9b, 0x3a2e3624, 0xb1670a0c, 0x0fe75793, 0xd296eeb4,
    0x9e919b1b, 0x4fc5c080, 0xa220dc61, 0x694b775a, 0x161a121c,
    0x0aba93e2, 0xe52aa0c0, 0x43e0223c, 0x1d171b12, 0x0b0d090e,
    0xadc78bf2, 0xb9a8b62d, 0xc8a91e14, 0x8519f157, 0x4c0775af,
    0xbbdd99ee, 0xfd607fa3, 0x9f2601f7, 0xbcf5725c, 0xc53b6644,
    0x347efb5b, 0x7629438b, 0xdcc623cb, 0x68fcedb6, 0x63f1e4b8,
    0xcadc31d7, 0x10856342, 0x40229713, 0x2011c684, 0x7d244a85,
    0xf83dbbd2, 0x1132f9ae, 0x6da129c7, 0x4b2f9e1d, 0xf330b2dc,
    0xec52860d, 0xd0e3c177, 0x6c16b32b, 0x99b970a9, 0xfa489411,
    0x2264e947, 0xc48cfca8, 0x1a3ff0a0, 0xd82c7d56, 0xef903322,
    0xc74e4987, 0xc1d138d9, 0xfea2ca8c, 0x360bd498, 0xcf81f5a6,
    0x28de7aa5, 0x268eb7da, 0xa4bfad3f, 0xe49d3a2c, 0x0d927850,
    0x9bcc5f6a, 0x62467e54, 0xc2138df6, 0xe8b8d890, 0x5ef7392e,
    0xf5afc382, 0xbe805d9f, 0x7c93d069, 0xa92dd56f, 0xb31225cf,
    0x3b99acc8, 0xa77d1810, 0x6e639ce8, 0x7bbb3bdb, 0x097826cd,
    0xf418596e, 0x01b79aec, 0xa89a4f83, 0x656e95e6, 0x7ee6ffaa,
    0x08cfbc21, 0xe6e815ef, 0xd99be7ba, 0xce366f4a, 0xd4099fea,
    0xd67cb029, 0xafb2a431, 0x31233f2a, 0x3094a5c6, 0xc066a235,
    0x37bc4e74, 0xa6ca82fc, 0xb0d090e0, 0x15d8a733, 0x4a9804f1,
    0xf7daec41, 0x0e50cd7f, 0x2ff69117, 0x8dd64d76, 0x4db0ef43,
    0x544daacc, 0xdf0496e4, 0xe3b5d19e, 0x1b886a4c, 0xb81f2cc1,
    0x7f516546, 0x04ea5e9d, 0x5d358c01, 0x737487fa, 0x2e410bfb,
    0x5a1d67b3, 0x52d2db92, 0x335610e9, 0x1347d66d, 0x8c61d79a,
    0x7a0ca137, 0x8e14f859, 0x893c13eb, 0xee27a9ce, 0x35c961b7,
    0xede51ce1, 0x3cb1477a, 0x59dfd29c, 0x3f73f255, 0x79ce1418,
    0xbf37c773, 0xeacdf753, 0x5baafd5f, 0x146f3ddf, 0x86db4478,
    0x81f3afca, 0x3ec468b9, 0x2c342438, 0x5f40a3c2, 0x72c31d16,
    0x0c25e2bc, 0x8b493c28, 0x41950dff, 0x7101a839, 0xdeb30c08,
    0x9ce4b4d8, 0x90c15664, 0x6184cb7b, 0x70b632d5, 0x745c6c48,
    0x4257b8d0
};

// key 256
u8 key[32] = {46, 11, 185, 88, 169, 98, 160, 5, 233, 204, 190, 208, 98, 158, 147, 
135, 70, 63, 115, 58, 205, 252, 7, 137, 22, 205, 253, 17, 240, 4, 175, 200};

static u32 shift(u32 r, int shift) {
    return (r >> shift) | (r << (sizeof (u32)*8 - shift));
}

static u32 FFmulX(u32 x) {
    return (((x & m2) << 1) ^ (((x & m1) >> 7) * m3));
}

static u32 FFmulX2(u32 x) {
    u32 t0 = (x & m5) << 2;
    u32 t1 = (x & m4);
    t1 ^= (t1 >> 1);
    return t0 ^ (t1 >> 2) ^ (t1 >> 5);
}

static u32 inv_mcol(u32 x) {
    u32 t0, t1;
    t0 = x;
    t1 = t0 ^ shift(t0, 8);
    t0 ^= FFmulX(t1);
    t1 ^= FFmulX2(t0);
    t0 ^= t1 ^ shift(t1, 16);
    return t0;
}

static u32 subWord(ctxAES* ctx, u32 x) {
    return (S[x & 255]&255 | ((S[(x >> 8)&255]&255) << 8) | ((S[(x >> 16)&255]&255) << 16) | S[(x >> 24)&255] << 24);
}

static u32 littleEndianToInt(u8 *bs, int off) {
    u32 n = bs[off] & 0xff;
    n |= (bs[++off] & 0xff) << 8;
    n |= (bs[++off] & 0xff) << 16;
    n |= bs[++off] << 24;
    return n;
}

static void generateWorkingKey(ctxAES* ctx) {
    // Key length not 128/192/256 bits.
    assert(ctx->aes_key_len == 16 || ctx->aes_key_len == 24 || ctx->aes_key_len == 32);

    int KC = ctx->aes_key_len >> 2;
    ctx->ROUNDS = KC + 6;

    switch (KC) {
        case 4:
        {
            u32 t0 = littleEndianToInt(key, 0);
            ctx->WK[0][0] = t0;
            u32 t1 = littleEndianToInt(key, 4);
            ctx->WK[0][1] = t1;
            u32 t2 = littleEndianToInt(key, 8);
            ctx->WK[0][2] = t2;
            u32 t3 = littleEndianToInt(key, 12);
            ctx->WK[0][3] = t3;

            for (int i = 1; i <= 10; ++i) {
                u32 u = subWord(ctx, shift(t3, 8)) ^ rcon[i - 1];
                t0 ^= u;
                ctx->WK[i][0] = t0;
                t1 ^= t0;
                ctx->WK[i][1] = t1;
                t2 ^= t1;
                ctx->WK[i][2] = t2;
                t3 ^= t2;
                ctx->WK[i][3] = t3;
            }

            break;
        }
        case 6:
        {
            u32 t0 = littleEndianToInt(key, 0);
            ctx->WK[0][0] = t0;
            u32 t1 = littleEndianToInt(key, 4);
            ctx->WK[0][1] = t1;
            u32 t2 = littleEndianToInt(key, 8);
            ctx->WK[0][2] = t2;
            u32 t3 = littleEndianToInt(key, 12);
            ctx->WK[0][3] = t3;
            u32 t4 = littleEndianToInt(key, 16);
            ctx->WK[1][0] = t4;
            u32 t5 = littleEndianToInt(key, 20);
            ctx->WK[1][1] = t5;

            u32 rcon = 1;
            u32 u = subWord(ctx, shift(t5, 8)) ^ rcon;
            rcon <<= 1;
            t0 ^= u;
            ctx->WK[1][2] = t0;
            t1 ^= t0;
            ctx->WK[1][3] = t1;
            t2 ^= t1;
            ctx->WK[2][0] = t2;
            t3 ^= t2;
            ctx->WK[2][1] = t3;
            t4 ^= t3;
            ctx->WK[2][2] = t4;
            t5 ^= t4;
            ctx->WK[2][3] = t5;

            for (int i = 3; i < 12; i += 3) {
                u = subWord(ctx, shift(t5, 8)) ^ rcon;
                rcon <<= 1;
                t0 ^= u;
                ctx->WK[i ][0] = t0;
                t1 ^= t0;
                ctx->WK[i ][1] = t1;
                t2 ^= t1;
                ctx->WK[i ][2] = t2;
                t3 ^= t2;
                ctx->WK[i ][3] = t3;
                t4 ^= t3;
                ctx->WK[i + 1][0] = t4;
                t5 ^= t4;
                ctx->WK[i + 1][1] = t5;
                u = subWord(ctx, shift(t5, 8)) ^ rcon;
                rcon <<= 1;
                t0 ^= u;
                ctx->WK[i + 1][2] = t0;
                t1 ^= t0;
                ctx->WK[i + 1][3] = t1;
                t2 ^= t1;
                ctx->WK[i + 2][0] = t2;
                t3 ^= t2;
                ctx->WK[i + 2][1] = t3;
                t4 ^= t3;
                ctx->WK[i + 2][2] = t4;
                t5 ^= t4;
                ctx->WK[i + 2][3] = t5;
            }

            u = subWord(ctx, shift(t5, 8)) ^ rcon;
            t0 ^= u;
            ctx->WK[12][0] = t0;
            t1 ^= t0;
            ctx->WK[12][1] = t1;
            t2 ^= t1;
            ctx->WK[12][2] = t2;
            t3 ^= t2;
            ctx->WK[12][3] = t3;

            break;
        }
        case 8:
        {
            u32 t0 = littleEndianToInt(key, 0);
            ctx->WK[0][0] = t0;
            u32 t1 = littleEndianToInt(key, 4);
            ctx->WK[0][1] = t1;
            u32 t2 = littleEndianToInt(key, 8);
            ctx->WK[0][2] = t2;
            u32 t3 = littleEndianToInt(key, 12);
            ctx->WK[0][3] = t3;
            u32 t4 = littleEndianToInt(key, 16);
            ctx->WK[1][0] = t4;
            u32 t5 = littleEndianToInt(key, 20);
            ctx->WK[1][1] = t5;
            u32 t6 = littleEndianToInt(key, 24);
            ctx->WK[1][2] = t6;
            u32 t7 = littleEndianToInt(key, 28);
            ctx->WK[1][3] = t7;

            u32 u, rcon = 1;

            for (int i = 2; i < 14; i += 2) {
                u = subWord(ctx, shift(t7, 8)) ^ rcon;
                rcon <<= 1;
                t0 ^= u;
                ctx->WK[i ][0] = t0;
                t1 ^= t0;
                ctx->WK[i ][1] = t1;
                t2 ^= t1;
                ctx->WK[i ][2] = t2;
                t3 ^= t2;
                ctx->WK[i ][3] = t3;
                u = subWord(ctx, t3);
                t4 ^= u;
                ctx->WK[i + 1][0] = t4;
                t5 ^= t4;
                ctx->WK[i + 1][1] = t5;
                t6 ^= t5;
                ctx->WK[i + 1][2] = t6;
                t7 ^= t6;
                ctx->WK[i + 1][3] = t7;
            }

            u = subWord(ctx, shift(t7, 8)) ^ rcon;
            t0 ^= u;
            ctx->WK[14][0] = t0;
            t1 ^= t0;
            ctx->WK[14][1] = t1;
            t2 ^= t1;
            ctx->WK[14][2] = t2;
            t3 ^= t2;
            ctx->WK[14][3] = t3;

            break;
        }
        default:
        {
            // Should never get here
            assert(0);
        }
    }

    for (int j = 0; j < ctx->ROUNDS + 1; j++)
        for (int i = 0; i < 4; i++)
            ctx->WK2[j][i] = ctx->WK[j][i];
    for (int j = 1; j < ctx->ROUNDS; j++) {
        for (int i = 0; i < 4; i++) {
            ctx->WK2[j][i] = inv_mcol(ctx->WK[j][i]);
        }
    }
}

static void unpackBlock(ctxAES* ctx, u8 *bytes) {
    int index = 0;
    ctx->C0 = (bytes[index++] & 0xff);
    ctx->C0 |= (bytes[index++] & 0xff) << 8;
    ctx->C0 |= (bytes[index++] & 0xff) << 16;
    ctx->C0 |= bytes[index++] << 24;

    ctx->C1 = (bytes[index++] & 0xff);
    ctx->C1 |= (bytes[index++] & 0xff) << 8;
    ctx->C1 |= (bytes[index++] & 0xff) << 16;
    ctx->C1 |= bytes[index++] << 24;

    ctx->C2 = (bytes[index++] & 0xff);
    ctx->C2 |= (bytes[index++] & 0xff) << 8;
    ctx->C2 |= (bytes[index++] & 0xff) << 16;
    ctx->C2 |= bytes[index++] << 24;

    ctx->C3 = (bytes[index++] & 0xff);
    ctx->C3 |= (bytes[index++] & 0xff) << 8;
    ctx->C3 |= (bytes[index++] & 0xff) << 16;
    ctx->C3 |= bytes[index++] << 24;
}

static void packBlock(ctxAES* ctx, u8 *bytes) {
    int index = 0;
    bytes[index++] = (u8) ctx->C0;
    bytes[index++] = (u8) (ctx->C0 >> 8);
    bytes[index++] = (u8) (ctx->C0 >> 16);
    bytes[index++] = (u8) (ctx->C0 >> 24);

    bytes[index++] = (u8) ctx->C1;
    bytes[index++] = (u8) (ctx->C1 >> 8);
    bytes[index++] = (u8) (ctx->C1 >> 16);
    bytes[index++] = (u8) (ctx->C1 >> 24);

    bytes[index++] = (u8) ctx->C2;
    bytes[index++] = (u8) (ctx->C2 >> 8);
    bytes[index++] = (u8) (ctx->C2 >> 16);
    bytes[index++] = (u8) (ctx->C2 >> 24);

    bytes[index++] = (u8) ctx->C3;
    bytes[index++] = (u8) (ctx->C3 >> 8);
    bytes[index++] = (u8) (ctx->C3 >> 16);
    bytes[index++] = (u8) (ctx->C3 >> 24);
}

static void encryptBlock(ctxAES* ctx) {
    u8* s = (u8*) S;

    u32 t0 = ctx->C0 ^ ctx->WK[0][0];
    u32 t1 = ctx->C1 ^ ctx->WK[0][1];
    u32 t2 = ctx->C2 ^ ctx->WK[0][2];

    u32 r = 1, r0, r1, r2, r3 = ctx->C3 ^ ctx->WK[0][3];
    while (r < ctx->ROUNDS - 1) {
        r0 = T0[t0 & 255] ^ shift(T0[(t1 >> 8)&255], 24) ^ shift(T0[(t2 >> 16)&255], 16) ^ shift(T0[(r3 >> 24)&255], 8) ^ ctx->WK[r][0];
        r1 = T0[t1 & 255] ^ shift(T0[(t2 >> 8)&255], 24) ^ shift(T0[(r3 >> 16)&255], 16) ^ shift(T0[(t0 >> 24)&255], 8) ^ ctx->WK[r][1];
        r2 = T0[t2 & 255] ^ shift(T0[(r3 >> 8)&255], 24) ^ shift(T0[(t0 >> 16)&255], 16) ^ shift(T0[(t1 >> 24)&255], 8) ^ ctx->WK[r][2];
        r3 = T0[r3 & 255] ^ shift(T0[(t0 >> 8)&255], 24) ^ shift(T0[(t1 >> 16)&255], 16) ^ shift(T0[(t2 >> 24)&255], 8) ^ ctx->WK[r++][3];
        t0 = T0[r0 & 255] ^ shift(T0[(r1 >> 8)&255], 24) ^ shift(T0[(r2 >> 16)&255], 16) ^ shift(T0[(r3 >> 24)&255], 8) ^ ctx->WK[r][0];
        t1 = T0[r1 & 255] ^ shift(T0[(r2 >> 8)&255], 24) ^ shift(T0[(r3 >> 16)&255], 16) ^ shift(T0[(r0 >> 24)&255], 8) ^ ctx->WK[r][1];
        t2 = T0[r2 & 255] ^ shift(T0[(r3 >> 8)&255], 24) ^ shift(T0[(r0 >> 16)&255], 16) ^ shift(T0[(r1 >> 24)&255], 8) ^ ctx->WK[r][2];
        r3 = T0[r3 & 255] ^ shift(T0[(r0 >> 8)&255], 24) ^ shift(T0[(r1 >> 16)&255], 16) ^ shift(T0[(r2 >> 24)&255], 8) ^ ctx->WK[r++][3];
    }

    r0 = T0[t0 & 255] ^ shift(T0[(t1 >> 8)&255], 24) ^ shift(T0[(t2 >> 16)&255], 16) ^ shift(T0[(r3 >> 24)&255], 8) ^ ctx->WK[r][0];
    r1 = T0[t1 & 255] ^ shift(T0[(t2 >> 8)&255], 24) ^ shift(T0[(r3 >> 16)&255], 16) ^ shift(T0[(t0 >> 24)&255], 8) ^ ctx->WK[r][1];
    r2 = T0[t2 & 255] ^ shift(T0[(r3 >> 8)&255], 24) ^ shift(T0[(t0 >> 16)&255], 16) ^ shift(T0[(t1 >> 24)&255], 8) ^ ctx->WK[r][2];
    r3 = T0[r3 & 255] ^ shift(T0[(t0 >> 8)&255], 24) ^ shift(T0[(t1 >> 16)&255], 16) ^ shift(T0[(t2 >> 24)&255], 8) ^ ctx->WK[r++][3];


    // the final round's table is a simple function of S so we don't use a whole other four tables for it

    ctx->C0 = (S[r0 & 255]&255) ^ ((S[(r1 >> 8)&255]&255) << 8) ^ ((s[(r2 >> 16)&255]&255) << 16) ^ (s[(r3 >> 24)&255] << 24) ^ ctx->WK[r][0];
    ctx->C1 = (s[r1 & 255]&255) ^ ((S[(r2 >> 8)&255]&255) << 8) ^ ((S[(r3 >> 16)&255]&255) << 16) ^ (s[(r0 >> 24)&255] << 24) ^ ctx->WK[r][1];
    ctx->C2 = (s[r2 & 255]&255) ^ ((S[(r3 >> 8)&255]&255) << 8) ^ ((S[(r0 >> 16)&255]&255) << 16) ^ (S[(r1 >> 24)&255] << 24) ^ ctx->WK[r][2];
    ctx->C3 = (s[r3 & 255]&255) ^ ((s[(r0 >> 8)&255]&255) << 8) ^ ((s[(r1 >> 16)&255]&255) << 16) ^ (S[(r2 >> 24)&255] << 24) ^ ctx->WK[r][3];
}

static void decryptBlock(ctxAES* ctx) {
    u8* s = (u8*) Si;

    u32 t0 = ctx->C0 ^ ctx->WK2[ctx->ROUNDS][0];
    u32 t1 = ctx->C1 ^ ctx->WK2[ctx->ROUNDS][1];
    u32 t2 = ctx->C2 ^ ctx->WK2[ctx->ROUNDS][2];

    u32 r = ctx->ROUNDS - 1, r0, r1, r2, r3 = ctx->C3 ^ ctx->WK2[ctx->ROUNDS][3];
    while (r > 1) {
        r0 = Tinv0[t0 & 255] ^ shift(Tinv0[(r3 >> 8)&255], 24) ^ shift(Tinv0[(t2 >> 16)&255], 16) ^ shift(Tinv0[(t1 >> 24)&255], 8) ^ ctx->WK2[r][0];
        r1 = Tinv0[t1 & 255] ^ shift(Tinv0[(t0 >> 8)&255], 24) ^ shift(Tinv0[(r3 >> 16)&255], 16) ^ shift(Tinv0[(t2 >> 24)&255], 8) ^ ctx->WK2[r][1];
        r2 = Tinv0[t2 & 255] ^ shift(Tinv0[(t1 >> 8)&255], 24) ^ shift(Tinv0[(t0 >> 16)&255], 16) ^ shift(Tinv0[(r3 >> 24)&255], 8) ^ ctx->WK2[r][2];
        r3 = Tinv0[r3 & 255] ^ shift(Tinv0[(t2 >> 8)&255], 24) ^ shift(Tinv0[(t1 >> 16)&255], 16) ^ shift(Tinv0[(t0 >> 24)&255], 8) ^ ctx->WK2[r--][3];
        t0 = Tinv0[r0 & 255] ^ shift(Tinv0[(r3 >> 8)&255], 24) ^ shift(Tinv0[(r2 >> 16)&255], 16) ^ shift(Tinv0[(r1 >> 24)&255], 8) ^ ctx->WK2[r][0];
        t1 = Tinv0[r1 & 255] ^ shift(Tinv0[(r0 >> 8)&255], 24) ^ shift(Tinv0[(r3 >> 16)&255], 16) ^ shift(Tinv0[(r2 >> 24)&255], 8) ^ ctx->WK2[r][1];
        t2 = Tinv0[r2 & 255] ^ shift(Tinv0[(r1 >> 8)&255], 24) ^ shift(Tinv0[(r0 >> 16)&255], 16) ^ shift(Tinv0[(r3 >> 24)&255], 8) ^ ctx->WK2[r][2];
        r3 = Tinv0[r3 & 255] ^ shift(Tinv0[(r2 >> 8)&255], 24) ^ shift(Tinv0[(r1 >> 16)&255], 16) ^ shift(Tinv0[(r0 >> 24)&255], 8) ^ ctx->WK2[r--][3];
    }

    r0 = Tinv0[t0 & 255] ^ shift(Tinv0[(r3 >> 8)&255], 24) ^ shift(Tinv0[(t2 >> 16)&255], 16) ^ shift(Tinv0[(t1 >> 24)&255], 8) ^ ctx->WK2[r][0];
    r1 = Tinv0[t1 & 255] ^ shift(Tinv0[(t0 >> 8)&255], 24) ^ shift(Tinv0[(r3 >> 16)&255], 16) ^ shift(Tinv0[(t2 >> 24)&255], 8) ^ ctx->WK2[r][1];
    r2 = Tinv0[t2 & 255] ^ shift(Tinv0[(t1 >> 8)&255], 24) ^ shift(Tinv0[(t0 >> 16)&255], 16) ^ shift(Tinv0[(r3 >> 24)&255], 8) ^ ctx->WK2[r][2];
    r3 = Tinv0[r3 & 255] ^ shift(Tinv0[(t2 >> 8)&255], 24) ^ shift(Tinv0[(t1 >> 16)&255], 16) ^ shift(Tinv0[(t0 >> 24)&255], 8) ^ ctx->WK2[r][3];

    // the final round's table is a simple function of Si so we don't use a whole other four tables for it

    ctx->C0 = (Si[r0 & 255]&255) ^ ((s[(r3 >> 8)&255]&255) << 8) ^ ((s[(r2 >> 16)&255]&255) << 16) ^ (Si[(r1 >> 24)&255] << 24) ^ ctx->WK2[0][0];
    ctx->C1 = (s[r1 & 255]&255) ^ ((s[(r0 >> 8)&255]&255) << 8) ^ ((Si[(r3 >> 16)&255]&255) << 16) ^ (s[(r2 >> 24)&255] << 24) ^ ctx->WK2[0][1];
    ctx->C2 = (s[r2 & 255]&255) ^ ((Si[(r1 >> 8)&255]&255) << 8) ^ ((Si[(r0 >> 16)&255]&255) << 16) ^ (s[(r3 >> 24)&255] << 24) ^ ctx->WK2[0][2];
    ctx->C3 = (Si[r3 & 255]&255) ^ ((s[(r2 >> 8)&255]&255) << 8) ^ ((s[(r1 >> 16)&255]&255) << 16) ^ (s[(r0 >> 24)&255] << 24) ^ ctx->WK2[0][3];
}

void aes_init(ctxAES* ctx) {
    ctx->aes_key_len = 32;
    u8 iv[16] = {(u8) 87, (u8) 239, (u8) 31, (u8) 187, (u8) 165, (u8) 213, (u8) 101, (u8) 191,
        (u8) 213, (u8) 82, (u8) 141, (u8) 250, (u8) 140, (u8) 84, (u8) 158, (u8) 92};
//    memcpy(ctx->iv, iv, sizeof(iv));
    generateWorkingKey(ctx);
    copy(iv, ctx->prev_block_c, 16);
    copy(iv, ctx->prev_block_d, 16);
}

void aes_encrypt(ctxAES* ctx, u8 *input, u8 *output, int len) {
    assert(len == 16); // 18 bytes == 128 bits
    unpackBlock(ctx, input);
    encryptBlock(ctx);
    packBlock(ctx, output);
}

void aes_decrypt(ctxAES* ctx, u8 *input, u8 *output, int len) {
    assert(len == 16); // 18 bytes == 128 bits
    unpackBlock(ctx, input);
    decryptBlock(ctx);
    packBlock(ctx, output);
}

void aes_encrypt_cbc(ctxAES* ctx, u8 *input, u8 *output, int len) {
    assert(len == 16); // 16 bytes == 128 bits
    XOR(input, ctx->prev_block_c, ctx->temp_block, len);
    aes_encrypt(ctx, ctx->temp_block, output, len);
    copy(output, ctx->prev_block_c, len);
}

void aes_decrypt_cbc(ctxAES* ctx, u8 *input, u8 *output, int len) {
    assert(len == 16); // 18 bytes == 128 bits
    aes_decrypt(ctx, input, ctx->temp_block, len);
    XOR(ctx->temp_block, ctx->prev_block_d, output, len);
    copy(input, ctx->prev_block_d, len);
}

void test_aes() {
    u8 x[16];
    u8 y[16];
    u8 z[16];
    ctxAES ctx;
    aes_init(&ctx);
    srand(time(0));
    int err = 0;
    for (int t = 0; t < 1e6; t++) {
        for (int i = 0; i < 16; i++) {
            x[i] = rand() % 256;
            //        printf("x[%i] = %i\n", i, x[i]);
        }

        aes_encrypt(&ctx, x, y, 16);
        aes_decrypt(&ctx, y, z, 16);
        for (int i = 0; i < 16; i++) {
            if (x[i] != z[i]) {
                err++;
                //            printf("x[%i] = %i \t z[%i] = %i\n", i, x[i], i, z[i]);
            }
        }
        assert(err == 0);
    }
    printf("OK\n");
}

void test_aes_iv() {
    //    u8 x[16];
    //    u8 xi[16];
    //    u8 y[16];
    //    u8 z[16];
    //    aes_init(AES_192);
    //    srand(time(0));
    //    int err = 0;
    //    for (int t = 0; t < 1e6; t++) {
    //        for (int i = 0; i < 16; i++) {
    //            x[i] = rand() % 256;
    //        }
    //        XOR(iv, x, xi, 16);
    //        aes_encrypt(xi, y, 16);
    //        for (int i = 0; i < 16; i++) {
    //            iv[i] = y[i];
    //        }
    //
    //        aes_decrypt(y, z, 16);
    //        XOR(iv2, z, z, 16);
    //        for (int i = 0; i < 16; i++) {
    //            iv2[i] = y[i];
    //        }
    //
    //        for (int i = 0; i < 16; i++) {
    //            if (x[i] != z[i]) {
    //                err++;
    //                printf("x[%i] = %i \t z[%i] = %i\n", i, x[i], i, z[i]);
    //            }
    //        }
    //        assert(err == 0);
    //    }
    //    printf("OK\n");
}

int main2(int argc, char** argv) {
    //    test_aes();
    test_aes_iv();
    return (EXIT_SUCCESS);
}