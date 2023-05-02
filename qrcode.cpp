#include <SFML/Graphics.hpp>
#include "qrcode.h"

vector<uint8_t>& QRCode::create_bitmap(string text, vector<uint8_t> &image_data, bool alpha) {
    vector<uint8_t> dark_pixel = { 0x00, 0x00, 0x00 }; // #000000 - black
    vector<uint8_t> light_pixel = { 0xFF, 0xFF, 0xFF }; // #FFFFFF - white

    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text.c_str(), qrcodegen::QrCode::Ecc::MEDIUM);
    rounded_size = round_image_size(qr);
    if (alpha) {
        // fully opaque for screen image
        dark_pixel.insert(dark_pixel.end(), 0xFF);
        light_pixel.insert(light_pixel.end(), 0xFF);
    }
    int qr_size = qr.getSize();
    // Image size with border / QR size with border
    int multiple_counter = size / (qr_size + 2);

    // top border
    for (int i = 0; i < rounded_size; i++) // row
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
    for (int i = 0; i < rounded_size; i++) // row
        for (int j = 0; j < multiple_counter; j++) // module pixel (height)
                image_data.insert(image_data.end(), light_pixel.begin(),
                                  light_pixel.end());

    return image_data;
}

bool QRCode::savePNG(string text, int target_size, string file_name) {
    vector<uint8_t> image_data;

    if (file_name.empty())

        return true;

    ifstream qr_file(file_name);
    if (qr_file.good()) {
        // file exists
        qr_file.close();

        return false;
    }
    qr_file.close();

    size = target_size;
    image_data = create_bitmap(text, image_data);
    ofstream png_file(file_name.c_str(), ios::binary);
    TinyPngOut pngout(rounded_size, rounded_size, png_file);
    // save to file
    pngout.write(image_data.data(), static_cast<size_t>(image_data.size() / 3));
    image_data.clear();
    png_file.close();

    return true;
}

uint32_t QRCode::round_image_size(qrcodegen::QrCode &qr) {
    return (size / (qr.getSize() + 2)) * (qr.getSize() + 2);
}

void QRCode::showImage(string text, int target_size) {
    vector<uint8_t> image_data;
    sf::Texture qr_image;
    sf::Sprite qr_sprite;

    size = target_size;
    image_data = create_bitmap(text, image_data, true);
    qr_image.create(rounded_size, rounded_size);
    qr_image.setSmooth(true);
    qr_image.update(&image_data[0]);
    qr_sprite.setTexture(qr_image);

    sf::RenderWindow window(sf::VideoMode(rounded_size, rounded_size), "QR code", sf::Style::Close);
    window.draw(qr_sprite);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

        }
        window.clear(sf::Color::White);
        window.draw(qr_sprite);
        window.display();
    }
}
