#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int main(void)
{
    /* A 256 bit key */
    unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

    /* A 128 bit IV */
    unsigned char *iv = (unsigned char *)"0123456789012345";

    /* Save the key to a file */
    FILE *key_file = fopen("key.txt", "wb");
    if (!key_file) {
        perror("Key file opening failed");
        return EXIT_FAILURE;
    }
    fwrite(key, sizeof(unsigned char), 32, key_file);
    fclose(key_file);

    /* Open the sample directory */
    const char *directory_path = "sample";
    DIR *dir = opendir(directory_path);
    if (!dir) {
        perror("Directory opening failed");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            /* Process only regular files */
            char input_filename[512];
            snprintf(input_filename, sizeof(input_filename), "%s/%s", directory_path, entry->d_name);

            /* Open the file to be encrypted */
            FILE *file = fopen(input_filename, "rb");
            if (!file) {
                perror("File opening failed");
                continue;
            }

            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            fseek(file, 0, SEEK_SET);

            unsigned char *plaintext = malloc(fsize + 1);
            fread(plaintext, 1, fsize, file);
            fclose(file);

            plaintext[fsize] = 0;

            /* Encrypt the plaintext */
            unsigned char *ciphertext = malloc(fsize + EVP_MAX_BLOCK_LENGTH);
            int ciphertext_len = encrypt(plaintext, fsize, key, iv, ciphertext);

            /* Create the encrypted file name */
            char output_filename[512];
            snprintf(output_filename, sizeof(output_filename), "%s/%s", directory_path, entry->d_name);

            /* Save the ciphertext to a file */
            FILE *enc_file = fopen(output_filename, "wb");
            if (!enc_file) {
                perror("Encrypted file opening failed");
                free(plaintext);
                free(ciphertext);
                continue;
            }
            fwrite(ciphertext, sizeof(unsigned char), ciphertext_len, enc_file);
            fclose(enc_file);

            /* Clean up */
            free(plaintext);
            free(ciphertext);
        }
    }

    closedir(dir);

    return 0;
}

