#include <iostream>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <string>
#include <vector>
#include <sstream>

///////////////////////////////////////
// Pseudo Random Number Generator
CryptoPP::AutoSeededRandomPool rng;


class Car {
 public:
    Car(const CryptoPP::RSA::PublicKey& publicKey) : 
        publicKey_(publicKey) {}

    std::string getMessage(const std::string& message) {
        srand(time(NULL));
        std::stringstream ss;
        ss << rand();

        sentMessage_ = ss.str();
        prevMessage_ = message;

        return encrypt(sentMessage_);
    }

    std::string challenge(const std::string& response) {
        if (response == sentMessage_ && prevMessage_ != "") {
            std::stringstream ss; 
            ss << "Ok, \"" << prevMessage_ << "\" complete.";

            sentMessage_ = "";
            prevMessage_ = "";

            return ss.str();
        }

        return "Sorry, challenge failed";
    }

 private:
    std::string encrypt(const std::string& message) const {
        std::string cipher;

        CryptoPP::RSAES_OAEP_SHA_Encryptor e2(publicKey_);

        CryptoPP::StringSource ss1(message, true,
            new CryptoPP::PK_EncryptorFilter(rng, e2,
                new CryptoPP::StringSink(cipher)
        ) // PK_EncryptorFilter
        ); // StringSource

        return cipher;
    }

    CryptoPP::RSA::PublicKey publicKey_;
    CryptoPP::AutoSeededRandomPool rng_;

    std::string prevMessage_;
    std::string sentMessage_;
};


class Trinket {
 public:
    Trinket(const CryptoPP::RSA::PrivateKey& privateKey) : 
        privateKey_(privateKey) {}

    std::string generateMessage() const noexcept{
        return "Open the door please";
    }

    std::string challenge(const std::string& response) const {
        return decrypt(response);
    }

 private:
    std::string decrypt(const std::string& message) const {
        std::string recovered;

        CryptoPP::RSAES_OAEP_SHA_Decryptor d2(privateKey_);

        CryptoPP::StringSource ss2(message, true,
            new CryptoPP::PK_DecryptorFilter(rng, d2,
                new CryptoPP::StringSink(recovered)
        ) // PK_DecryptorFilter
        ); // StringSource

        return recovered;
    }

    CryptoPP::RSA::PrivateKey privateKey_;
};


int main() {
    ///////////////////////////////////////
    // Generate Parameters
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, 1024*4);

    /*
    const CryptoPP::Integer& n = params.GetModulus();
    const CryptoPP::Integer& p = params.GetPrime1();
    const CryptoPP::Integer& q = params.GetPrime2();
    const CryptoPP::Integer& d = params.GetPrivateExponent();
    const CryptoPP::Integer& e = params.GetPublicExponent();

    ///////////////////////////////////////
    // Dump
    std::cout << "RSA Parameters:" << std::endl;
    std::cout << " n: " << n << std::endl;
    std::cout << " p: " << p << std::endl;
    std::cout << " q: " << q << std::endl;
    std::cout << " d: " << d << std::endl;
    std::cout << " e: " << e << std::endl;
    std::cout << std::endl;
    */

    ///////////////////////////////////////
    // Create Keys
    CryptoPP::RSA::PrivateKey privateKey(params);
    CryptoPP::RSA::PublicKey publicKey(params);

    Car car(publicKey);
    Trinket trinket(privateKey);

    std::string trinketSentMessage = trinket.generateMessage();
    std::string carResponse = car.getMessage(trinketSentMessage);
    std::string trinketResponse = trinket.challenge(carResponse);
    std::string carChalengeResponse = car.challenge(trinketResponse);

    std::cout << "[Trinket -> Car] Trinket send command: " << trinketSentMessage << std::endl
              << "[Car -> Trinket] Car generate random value, encode and response: " << "******************************************" << std::endl
              << "[Trinket -> Car] Trinket deconde car's random value and send back: " << trinketResponse << std::endl
              << "[Car -> Trinket] Car check equal: " << carChalengeResponse << std::endl;
}