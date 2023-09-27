#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "SSIM.hpp"

#define MD5_DIGEST_LENGTH 16
#define BUFFER_SIZE 8192

void getMD5hash(const std::string &file_path, unsigned char *hash, unsigned int *hashLen) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX *md5ptr = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5ptr, EVP_md5(), nullptr);
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while (file.read(reinterpret_cast<char *>(buffer), BUFFER_SIZE)) {
        bytes_read = file.gcount();
        EVP_DigestUpdate(md5ptr, buffer, bytes_read);
    }
    int success = EVP_DigestFinal_ex(md5ptr, hash, hashLen);
    EVP_MD_CTX_free(md5ptr);
    file.close();
    if (!success) {
        exit(EXIT_FAILURE);
    }
}

void compare_hashes(const std::string &dir1, const std::string &dir2) {
    namespace fs = std::filesystem;

    for (const auto &entry1 : fs::directory_iterator(dir1)) {
        if (entry1.is_directory()) {
            continue; // Skip directories
        }

        std::string file_path1 = entry1.path().string();
        unsigned char hash1[MD5_DIGEST_LENGTH];
        unsigned int hash1Len;
        getMD5hash(file_path1, hash1, &hash1Len);

        for (const auto &entry2 : fs::directory_iterator(dir2)) {
            if (entry2.is_directory()) {
                continue; // Skip directories
            }

            std::string file_path2 = entry2.path().string();
            unsigned char hash2[MD5_DIGEST_LENGTH];
            unsigned int hash2Len;
            getMD5hash(file_path2, hash2, &hash2Len);

            if (std::memcmp(hash1, hash2, MD5_DIGEST_LENGTH) == 0) {
                std::cout << "Files " << file_path1 << " and " << file_path2 << " have the same hash." << std::endl;
            } else {
                double ssim = compare_image(file_path1.c_str(), file_path2.c_str());
                if (ssim > 0.7) {
                    std::cout << file_path1 << " - " << file_path2 << " SSIM: " << ssim << std::endl;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <dir1> <dir2>" << std::endl;
        return 1;
    }
   
    std::string dir1 = argv[1];
    std::string dir2 = argv[2];
    compare_hashes(dir1, dir2);
    return 0;
}
