#include "CLI11.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include <cstdio>
#include <unistd.h>
#include <boost/filesystem.hpp>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int InstallWine(std::string version) {
    CURL *curl;
    FILE *fp;
    CURLcode res;

    std::string urlTemplate { "https://dl.winehq.org/wine/source/%major%.x/wine-%version%.tar.xz" };
    urlTemplate.replace(34, 7, version.substr(0, 1));
    urlTemplate.replace(43, 9, version);

    std::string filename { "wine-%version%.tar.xz" };
    filename.replace(5, 9, version);

    std::cout << "installing " << urlTemplate << "...\n";
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, urlTemplate.data());
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    
    if(res == CURLE_HTTP_RETURNED_ERROR) {
        std::remove(filename.c_str());
        return 0;
    } else {
        return 1;
    }
}

int main(int argc, char** argv) {
    #ifdef DEBUG
        std::cout << "This is a development build - dont release this\n\n";
    #endif

    CLI::App app{"wvm - Wine version manager"};

    app.set_version_flag("--version", "1.0");

    std::string version = "7.12";
    CLI::Option* installOpt { app.add_option("-i,--install", version, "Install specific version of wine") };

    bool win32 { false };
    app.add_flag("--32,-b", win32, "Built for windows 32. Default is windows 64");

    int parallelism {};
    app.add_option("-j", parallelism, "Amount of cores to run in parallel");

    CLI11_PARSE(app, argc, argv);

    std::cout << app.get_description() << "\n\n";

    if(* installOpt) {
        if(InstallWine(version)) {
            std::cout << "Succesfully installed wine version \"" << version << "\"\n";
        } else {
            std::cout << "Failed to install wine version \"" << version << "\"\n\nExiting...\n";
            return EXIT_FAILURE;
        }

        std::string cmdTemplate { "tar -xf " };
        std::string filename { "wine-%version%.tar.xz" };
        filename.replace(5, 9, version);
        cmdTemplate.append(filename);

        std::cout << "Extracting files...\n";
        std::system(cmdTemplate.c_str());
        std::remove(filename.c_str());
        std::cout << "Succesfully extracted files...\n";

        if(win32)
            std::cout << "Building wine for win32...\n";
        else
            std::cout << "Building wine for win64...\n";
        std::system("mkdir build");
        chdir("build");
        filename.replace(5 + version.length(), 8, "");
        cmdTemplate = "../x/configure";
        cmdTemplate.replace(3, 1, filename);
        if(!win32)
            cmdTemplate.append(" --enable-win64");
        std::system(cmdTemplate.c_str());
        cmdTemplate = "make -j";
        if(parallelism > 0)
            cmdTemplate.append(std::to_string(parallelism));
        else
            cmdTemplate.erase(5, 3);
        std::system(cmdTemplate.c_str());
        filename.insert(0, "../");
        boost::filesystem::remove_all(filename.c_str());
        std::cout << "Succesfully build wine\n";
    }

    return EXIT_SUCCESS;
}
