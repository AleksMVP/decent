#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


struct LaunchArguments {
    std::string fileName;
    int numBilets;
    uint32_t parametr;
};

LaunchArguments parseArgs(int argc, char* argv[]) {
    LaunchArguments args;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--file") {
            args.fileName = argv[i + 1];
        } else if (std::string(argv[i]) == "--numbilets") {
            args.numBilets = std::stoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "--parametr") {
            args.parametr = std::stoi(argv[i + 1]);
        }
    }

    return args;
}


class BiletRandomizer {
 public:
    BiletRandomizer(const LaunchArguments& args) : args_(args) {
        stream_.open(args.fileName, std::fstream::in);
    }

    std::string computeAndReturnLine() {
        std::string line;
        std::stringstream ss;
        std::getline(stream_, line);

        ss << line << ": " << compute(line) % args_.numBilets + 1;

        return ss.str();
    }

    bool isEof() const noexcept {
        return stream_.eof();
    }

    ~BiletRandomizer() noexcept {
        stream_.close();
    }

 private:
    long long compute(const std::string& line) {
        std::stringstream ss;
        ss << args_.parametr;
        return static_cast<long long>(std::hash<std::string>{}(ss.str())) ^
                    static_cast<long long>(std::hash<std::string>{}(line));
    }

    LaunchArguments args_;
    std::fstream stream_;
};


int main(int argc, char* argv[]) {
    LaunchArguments args = parseArgs(argc, argv);
    std::cout << args.fileName << " " 
              << args.numBilets << " " 
              << args.parametr << std::endl;

    BiletRandomizer instance(args);
    while (!instance.isEof()) {
      std::cout << instance.computeAndReturnLine() << std::endl;  
    }
}