#include "qrcode.h"

QRCode::QRCode(int image_size) {
    size = image_size;
}

vector<uint8_t>& QRCode::create_bitmap(qrcodegen::QrCode &qr, vector<uint8_t> &image_data, bool alpha) {
    vector<uint8_t> dark_pixel = { 0x00, 0x00, 0x00 }; // #000000 - black
    vector<uint8_t> light_pixel = { 0xFF, 0xFF, 0xFF }; // #FFFFFF - white

    round_size = round_image_size(qr);
    if (alpha) {
        // fully opaque for screen image
        dark_pixel.insert(dark_pixel.end(), 0xFF);
        light_pixel.insert(light_pixel.end(), 0xFF);
    }
    int qr_size = qr.getSize();
    // Image size with border / QR size with border
    int multiple_counter = size / (qr_size + 2);

    // top border
    for (int i = 0; i < round_size; i++) // row
        for (int j = 0; j < multiple_counter; j++) // module pixel (height)
                image_data.insert(image_data.end(), light_pixel.begin(),
                                  light_pixel.end());

    // qr code
    for (int qr_module_y = 0; qr_module_y < qr_size; qr_module_y++) {
        for (int col = 0; col < multiple_counter; col++) {
            // left border
            for (int i = 0; i < multiple_counter; ++i)
                image_data.insert(image_data.end(), light_pixel.begin(),
                                  light_pixel.end());
            // qr pixel to image pixels
            for (int qr_module_x = 0; qr_module_x < qr_size; qr_module_x++) {
                for (int row = 0; row < multiple_counter; ++row) {
                     // get qr module at x:y
                    if (qr.getModule(qr_module_x, qr_module_y)) {
                        image_data.insert(image_data.end(), dark_pixel.begin(),
                                          dark_pixel.end());
                    } else {
                        image_data.insert(image_data.end(), light_pixel.begin(),
                                          light_pixel.end());
                    }
                }
            }
            // right border
            for (int i = 0; i < multiple_counter; ++i)
                image_data.insert(image_data.end(), light_pixel.begin(),
                                  light_pixel.end());
        }
    }

    // bottom border
    for (int i = 0; i < round_size; i++) // row
        for (int j = 0; j < multiple_counter; j++) // module pixel (height)
                image_data.insert(image_data.end(), light_pixel.begin(),
                                  light_pixel.end());

    return image_data;
}

bool QRCode::savePNG(string text, string file_name) {
    vector<uint8_t> image_data;

    if (file_name.empty())

        return false;

    ifstream qr_file(file_name);
    if (qr_file.good()) {
        // file exists
        qr_file.close();

        return false;
    }
    qr_file.close();

    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text.c_str(), qrcodegen::QrCode::Ecc::MEDIUM);
    image_data = create_bitmap(qr, image_data);

    ofstream png_file(file_name.c_str(), ios::binary);
    TinyPngOut pngout(round_size, round_size, png_file);
    // save to file
    pngout.write(image_data.data(), static_cast<size_t>(image_data.size() / 3));
    image_data.clear();
    png_file.close();

    return true;
}

uint32_t QRCode::round_image_size(qrcodegen::QrCode &qr) {
    return (size / (qr.getSize() + 2)) * (qr.getSize() + 2);
}
