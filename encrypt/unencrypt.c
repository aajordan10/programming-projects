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

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main(void)
{
    /* Key and IV */
    unsigned char key[32];
    unsigned char *iv = (unsigned char *)"0123456789012345";

    /* Read the key from key.txt */
    FILE *key_file = fopen("key.txt", "rb");
    if (!key_file) {
        perror("Key file opening failed");
        return EXIT_FAILURE;
    }
    fread(key, sizeof(unsigned char), 32, key_file);
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
        if (strstr(entry->d_name, "_encrypted")) {
            /* Process only files containing "_encrypted" */
            char input_filename[512];
            snprintf(input_filename, sizeof(input_filename), "%s/%s", directory_path, entry->d_name);

            /* Open the encrypted file */
            FILE *enc_file = fopen(input_filename, "rb");
            if (!enc_file) {
                perror("Encrypted file opening failed");
                continue;
            }

            fseek(enc_file, 0, SEEK_END);
            long enc_size = ftell(enc_file);
            fseek(enc_file, 0, SEEK_SET);

            unsigned char *ciphertext = malloc(enc_size);
            fread(ciphertext, 1, enc_size, enc_file);
            fclose(enc_file);

            /* Buffer for the decrypted text */
            unsigned char *decryptedtext = malloc(enc_size + EVP_MAX_BLOCK_LENGTH);

            /* Decrypt the ciphertext */
            int decryptedtext_len = decrypt(ciphertext, enc_size, key, iv, decryptedtext);
            decryptedtext[decryptedtext_len] = '\0';

            /* Create the decrypted file name */
            char output_filename[512];
            snprintf(output_filename, sizeof(output_filename), "%s/%s_unencrypted.txt", directory_path, entry->d_name);

            /* Save the decrypted text to a new file */
            FILE *dec_file = fopen(output_filename, "wb");
            if (!dec_file) {
                perror("Decrypted file opening failed");
                free(ciphertext);
                free(decryptedtext);
                continue;
            }
            fwrite(decryptedtext, sizeof(unsigned char), decryptedtext_len, dec_file);
            fclose(dec_file);

            /* Clean up */
            free(ciphertext);
            free(decryptedtext);
        }
    }

    closedir(dir);

    return 0;
}

