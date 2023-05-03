#include <iostream>
#include <string>
#include <filesystem>

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

typedef std::string String;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "give a video path\n";
        return 1;
    }

    bool is_windows = false;
    #ifdef _WIN32
        is_windows = true
    #endif

    const String video_path = argv[1];

    cv::VideoCapture cap;
    cap.open(video_path);
    if (!cap.isOpened()) {
        std::cerr << "could not open file `" << video_path << "`\n";
        return 1;
    }

    const String::size_type ext_pos = video_path.rfind(".");
    const String dir_prefix = video_path.substr(0, ext_pos);
    String output_dir = dir_prefix + "-frames";
    if (argc == 3) {
        output_dir = argv[2];
        if (!fs::exists(output_dir)) {
            std::cerr << "no such directory `" << output_dir << "`\n";
            return 1;
        }
    }
    else if (!fs::create_directory(output_dir)) {
        std::cerr << "error creating directory `" << output_dir << "`\n";
        return 1;
    }
    const String sep = is_windows ? "\\" : "/";
    const String file_prefix = output_dir + sep + "frame_";

    unsigned long frame_idx = 0;
    const unsigned long num_frames = static_cast<unsigned long>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    cv::Mat frame;
    bool is_ok = true;
    std::cout << "capturing frames in `" << video_path << "`" << std::endl;
    while (true) {
        cap.read(frame);
        if (frame.empty()) break;

        const String filename = file_prefix + std::to_string(++frame_idx) + ".png";
        try {
            is_ok = cv::imwrite(filename, frame);
        }
        catch (const cv::Exception &excpt) {
            std::cerr << "exception occured: " << excpt.what() << "\n";
            is_ok = false;
            break;
        }

        if (!is_ok) {
            std::cerr << "could not save file `" << filename << "`\n";
            break;
        }

        std::cout << frame_idx << "/" << num_frames << "\r";
    }
    std::cout << std::endl;

    return is_ok ? 0 : 1;
}