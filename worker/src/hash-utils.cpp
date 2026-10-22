#include "../include/hash-utils.h"

#include <openssl/evp.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>

std::string sha256(const std::string& input){

        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digestLength = 0;

        if (EVP_Digest(
                    input.data(),
                    input.size(),
                    digest,
                    &digestLength,
                    EVP_sha256(),
                    nullptr) != 1) {
                throw std::runtime_error("SHA-256 computation failed");
            }

        std::ostringstream output;

        for (unsigned int i = 0; i < digestLength; ++i) {
               output << std::hex
                      << std::setw(2)
                      << std::setfill('0')
                      << static_cast<unsigned int>(digest[i]);
           }

           return output.str();



}
