#ifndef QRCODE_H
#define QRCODE_H

#include "libs/QRCodeGen/qrcodegen.hpp"
#include "libs/TinyPNGOut/TinyPngOut.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

class QRCode {
public:
    QRCode() {};

    bool savePNG(string text, int size, string filename);
    void showImage(string text, int size);

private:
    int size;
    int rounded_size;

    uint32_t round_image_size(qrcodegen::QrCode &qr);
    vector<uint8_t>& create_bitmap(string text, vector<uint8_t> &image_data, bool alpha = false);
};

#endif // QRCODE_H
