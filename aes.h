#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif

    enum AES_type {
        AES_128,
        AES_192,
        AES_256
    };

    typedef struct {
        int aes_key_len;
        int ROUNDS;
        u32 WK[20][4];
        u32 WK2[20][4];
        u32 C0, C1, C2, C3;




//        u8 iv[16];
        u8 prev_block_c[16];
        u8 prev_block_d[16];
        u8 temp_block[16];
    } ctxAES;

    void aes_encrypt(ctxAES* ctx, u8 *input, u8 *output, int len);
    void aes_decrypt(ctxAES* ctx, u8 *input, u8 *output, int len);
    void aes_init(ctxAES* ctx);

    void aes_encrypt_cbc(ctxAES* ctx, u8 *input, u8 *output, int len);
    void aes_decrypt_cbc(ctxAES* ctx, u8 *input, u8 *output, int len);

#ifdef __cplusplus
}
#endif

#endif /* AES_H */

