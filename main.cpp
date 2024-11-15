//
// Created by user on 12/13/23.
//
#include "binary_utils.h"
#include "blind_watermark_core0.h"
using namespace re;

const std::string DEFAULT_INPUT_PATH = "/Users/zhangyan/Documents/images/blindwm/3.jpeg";
const std::string DEFAULT_OUTPUT_PATH = "/Users/zhangyan/Documents/images/blindwm/with_watermark.jpeg";
const std::string DEFAULT_WATERMARK = "AIGC";
const int DEFAULT_WATERMARK_BIN_SIZE = 64;  // 默认水印二进制大小

// 计算水印的二进制长度
int calculate_watermark_bin_size(const std::string& wm_encode) {
  auto wm_bits = BinaryUtils::utf8StrToBinArray(wm_encode);
  auto xx = cv::Mat(wm_bits);
  std::cout << "wm_bits size: " << wm_bits.size() << std::endl;
  return wm_bits.size();
}

void extract_watermark(const std::string& input_path, int watermark_bin_size) {
  auto core = std::make_unique<BindWatermarkCoreV0>();
  cv::Mat emb_img = cv::imread(input_path);
  auto extract_bits = core->extract(emb_img, watermark_bin_size);
  auto extract_str = BinaryUtils::binArrayToUtf8Str(extract_bits);
  std::cout << "extract_str: " << extract_str << std::endl;
}

int embed_watermark(const std::string& input_path, const std::string& output_path, const std::string& watermark) {

  cv::Mat img = cv::imread(input_path, cv::IMREAD_UNCHANGED); 
  auto wm_bits = BinaryUtils::utf8StrToBinArray(watermark);
  auto xx = cv::Mat(wm_bits);
  std::cout << "wm_bits size: " << wm_bits.size() << std::endl;

  auto core = std::make_unique<BindWatermarkCoreV0>();
  core->readImage(img);
  core->readWatermark(wm_bits);
  auto embed_img = core->embed();
  std::cout << "embed_img size: " << embed_img.size() << std::endl;

  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_JPEG_QUALITY); // JPEG 质量参数
  compression_params.push_back(100); // 质量数值，0-100，越高质量越好
  cv::imwrite(output_path, embed_img, compression_params);

  return wm_bits.size();
}

void maintest() {
  auto img_path = "/Users/user/Downloads/ori_img.jpeg";
  cv::Mat img = cv::imread(img_path);
  auto wm_bits = BinaryUtils::utf8StrToBinArray("jiemo");
  auto xx = cv::Mat(wm_bits);
  std::cout << "wm_bits size: " << wm_bits.size() << std::endl;

  auto core = std::make_unique<BindWatermarkCoreV0>();
  core->readImage(img);
  core->readWatermark(wm_bits);
  auto embed_img = core->embed();
  std::cout << "embed_img size: " << embed_img.size() << std::endl;

  cv::imwrite("embed_img.png", embed_img);

  std::cout << "-------------------------------------\n";
  cv::Mat emb_img = cv::imread("embed_img.png");
  auto extract_bits = core->extract(emb_img, wm_bits.size());
  auto extract_str = BinaryUtils::binArrayToUtf8Str(extract_bits);
  std::cout << "extract_str: " << extract_str << std::endl;
}

int main(int argc, char* argv[]) {
    std::string operation = ""; // 默认操作
    std::string watermark = DEFAULT_WATERMARK;
    std::string input_path = DEFAULT_INPUT_PATH;
    std::string output_path = DEFAULT_OUTPUT_PATH;
    int watermark_bin_size = DEFAULT_WATERMARK_BIN_SIZE;  // 默认水印二进制大小

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--op" && i + 1 < argc) {
            operation = argv[++i];
        } else if (arg == "--wm" && i + 1 < argc) {
            watermark = argv[++i];
        } else if (arg == "--in" && i + 1 < argc) {
            input_path = argv[++i];
        } else if (arg == "--out" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--size" && i + 1 < argc) {
            watermark_bin_size = std::stoi(argv[++i]); // 解析并设置水印二进制大小
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 根据操作执行
    if (operation == "embed") {
        watermark_bin_size = embed_watermark(input_path, output_path, watermark);
        std::cout << "Watermark size: " << watermark_bin_size << std::endl;
    } else if (operation == "extract") {
        extract_watermark(input_path, watermark_bin_size);
    } else if (operation == "info") {
        // 仅显示水印的二进制长度
        watermark_bin_size = calculate_watermark_bin_size(watermark);
        std::cout << "Watermark binary size: " << watermark_bin_size << std::endl;
    } else {
        watermark_bin_size = embed_watermark(input_path, output_path, watermark);
        std::cout << "Watermark size: " << watermark_bin_size << std::endl;
        extract_watermark(output_path, watermark_bin_size);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "cost : " << elapsed.count() << " ms" << std::endl;

    return 0;
}