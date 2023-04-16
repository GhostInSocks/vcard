#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>
#include "vcard.h"

using namespace std;

Vcard::Vcard() {}

string Vcard::base64_encode(vector<unsigned char> in) {
    string out;

    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
                          [(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
                                 [((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');

    return out;
}

string Vcard::createVCF() {
    // https://www.rfc-editor.org/rfc/rfc6350
    string vcf = BEGIN + "\n";

    vcf += VERSION + "\n";
    vcf += fn + "\n";
    if (!n.empty()) vcf += n + "\n";
    if (!addr.empty()) vcf += addr + "\n";
    if (!bday.empty()) vcf += bday + "\n";
    if (!email.empty()) vcf += email + "\n";
    if (!note.empty()) vcf += note + "\n";
    if (!org.empty()) vcf += org + "\n";
    if (!photo.empty()) vcf += photo + "\n";
    if (!tel_home.empty()) vcf += tel_home + "\n";
    if (!tel_work.empty()) vcf += tel_work + "\n";
    if (!title.empty()) vcf += title + "\n";
    if (!url.empty()) vcf += url + "\n";
    vcf += END + "\n";

    return vcf;
}

string Vcard::splitName(string name) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.2.2
    stringstream ss;
    string part, rez;

    name = oneSpace(name);
    ss.str(name);
    while (getline(ss, part, ' ')) {
        if (!rez.empty()) rez += ",";
        rez += part;
    }

    return rez;
}

string Vcard::clearSpaces(string data) {
    // Remove space chars: \t\r\n\f
    return regex_replace(data, regex("\\s+"), "");
}

string Vcard::oneSpace(string data) {
    // Remove spaces on left
    data = regex_replace(data, regex("^\\s+"), "");
    // Remove spaces on right
    data = regex_replace(data, regex("\\s+$"), "");
    // Escaping chars: comma, semicolon, backslash
    // https://www.rfc-editor.org/rfc/rfc6350.html#section-3.4
    data = regex_replace(data, regex(","), "\\,");
    data = regex_replace(data, regex(";"), "\\;");
    data = regex_replace(data, regex("\\\\"), "\\\\");

    // Remove duplicated spaces
    return regex_replace(data, regex("\\s+"), " ");
}

void Vcard::addPerson(string prefix, string given_name, string middle_name,
               string family_name, string suffix) {
    bool space = false;

    // https://www.rfc-editor.org/rfc/rfc6350#section-6.2.2
    if (given_name.empty() && family_name.empty())

        return;

    n = "N:" + splitName(family_name) + ";" + splitName(given_name) + ";" +
            splitName(middle_name) + ";" + prefix + ";" + suffix;
    n = clearSpaces(n);
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.2.1
    fn = "FN:";
    if (!prefix.empty()) {
        fn += clearSpaces(prefix);
        space = true;
    }
    if (!given_name.empty()) {
        if (space) fn += " ";
        fn += given_name;
        space = true;
    }
    if (!middle_name.empty()) {
        if (space) fn += " ";
        fn += middle_name;
        space = true;
    }
    if (!family_name.empty()) {
        if (space) fn += " ";
        fn += family_name;
        space = true;
    }
    if (!suffix.empty()) {
        if (space) fn += " ";
        fn += clearSpaces(suffix);
    }
    fn = oneSpace(fn);
}

bool Vcard::isDigit(string data) {
    // string contains only digits
    return data.find_first_not_of( "0123456789" ) == string::npos;
}

bool Vcard::addBday(string bdate) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.2.5
    stringstream ss;
    string day, month, year;

    if (bdate.empty()) return true;

    bdate = clearSpaces(bdate);
    ss.str(bdate);
    getline(ss, day, '.');
    if (!isDigit(day) || day.length() != 2 || stoi(day) < 1 ||
            stoi(day) > 31) return false;
    getline(ss, month, '.');
    if (!isDigit(month) || month.length() != 2 || stoi(month) < 1 ||
            stoi(month) > 12) return false;
    getline(ss, year, '.');
    if (!isDigit(year) || year.length() != 4) return false;
    bday = "BDAY:" + year + "-" + month + "-" + day;

    return true;
}

bool Vcard::addAddress(string street_house, string city, string state,
                       string zip, string country) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.3.1
    if (street_house.empty() || city.empty() || country.empty())

        return true;

    zip = oneSpace(zip);
    if (!isDigit(zip) && !zip.empty())

        return false;

    addr = "ADR:;;" + oneSpace(street_house) + ";" + oneSpace(city) + ";" +
            oneSpace(state) + ";" + zip + ";" + oneSpace(country);

    return true;
}

bool Vcard::checkPhone(string phone) {
    if (regex_match(phone, regex("(\\(?\\+?[0-9]{1,3}\\)?"
        "[ \\.\\-]*)?\\(?[0-9]{1,3}\\)?[ \\.\\-]*[ \\.\\-0-9]{6,}"
        "(([ \\.\\-]*)?\\(?(ext)?[ \\.\\:]*[0-9]{1,}\\)?)?")))

        return true;

    return false;
}

bool Vcard::addHomePhone(string phone) {
    if (phone.empty())

        return true;

    if (!checkPhone(phone))

        return false;

    phone = oneSpace(phone);
    tel_home = "TEL;VALUE=uri;TYPE=home:tel:" + phone;

    return true;
}

bool Vcard::addWorkPhone(string phone) {
    string ext;
    smatch match;

    if (phone.empty())

        return true;

    if (!checkPhone(phone))

        return false;

    phone = oneSpace(phone);
    // Searching phone
    if (regex_search(phone, match, regex("(\\(?\\+?[0-9]{1,3}\\)?"
        "[ \\.\\-]*)?\\(?[0-9]{1,3}\\)?[ \\.\\-]*[ \\.\\-0-9]{6,}")))
        tel_work = "TEL;VALUE=uri;TYPE=work:tel:" + oneSpace(match[0]);

    // Searching for extention
    if (regex_search(phone, match, regex("(\\(|(ext)[ \\.\\:]*)[0-9]{1,}\\)?$"))) {
        ext = match[0];
        if (regex_search(ext, match, regex("[0-9]{1,}")))
            tel_work += ";ext=" + string(match[0]);
    }

    return true;
}

void Vcard::addBussiness(string organization, string function) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.6.4
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.6.1
    if (!organization.empty()) org = "ORG:" + oneSpace(organization);
    if (!function.empty()) title = "TITLE:" + oneSpace(function);
}

void Vcard::addNote(string comment) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.7.2
    if (!comment.empty()) note = "NOTE:" + oneSpace(comment);
}

bool Vcard::addEmail(string mail) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.4.2
    if (mail.empty())

        return true;

    mail = clearSpaces(mail);
    if (!regex_match(mail, regex("^[\\w\\-\\.]+@([\\w\\-]+\\.)+[\\w\\-]{2,4}$")))

        return false;

    email = "EMAIL:" + mail;

    return true;
}

bool Vcard::addUrl(string link) {
    // https://www.rfc-editor.org/rfc/rfc6350#section-6.7.8
    if (link.empty())

        return true;

    link = clearSpaces(link);
    if (!regex_match(link, regex("[(http(s)?):\\/\\/(www\\.)?a-zA-Z0-9@:%._\\+~#=]{2,256}"
                                 "\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)")))

        return false;

    url = "URL:" + link;

    return true;
}

bool Vcard::addPhoto(string image) {
    string file_name;

    if (image.empty())

        return true;

    image = clearSpaces(image);
    // Check file exists
    file_name = regex_replace(image, regex("/"), "//");
    ifstream file(file_name);
    if (file.good()) {
        photo = "PHOTO:data:image/jpeg;base64," + base64_encode(readFile(file_name));
        file.close();

        return true;
    }
    file.close();

    // Check URL
    if (regex_match(image, regex("[(http(s)?):\\/\\/(www\\.)?a-zA-Z0-9@:%._\\+~#=]{2,256}"
                                 "\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"))) {
        photo = "PHOTO:" + image;

        return true;
    }

    return false;
}

vector<unsigned char> Vcard::readFile(string file_name) {
    vector<unsigned char> buf;
    ifstream file(file_name, ios::binary);
    streampos file_size;

    // do not skip spaces
    file.unsetf(ios::skipws);
    // get size
    file.seekg(0, ios::end);
    file_size = file.tellg();
    file.seekg(0, ios::beg);
    // reserve space
    buf.reserve(file_size);
    // read the data:
    buf.insert(buf.begin(), istream_iterator<unsigned char>(file),
               istream_iterator<unsigned char>());
    file.close();

    return buf;
}

bool Vcard::saveVCF(string file_name) {
    if (file_name.empty()) {
        cout << "Ime datoteke ne sme biti prazno" << endl;

        return false;
    }
    file_name = regex_replace(file_name, regex("/"), "//");
    ifstream ifile(file_name);
    if (ifile.good()) {
        ifile.close();
        cout << "Datoteka" + file_name + "ze obstaja" << endl;

        return false;
    }
    ifile.close();
    ofstream ofile(file_name);
    ofile << createVCF();
    ofile.close();

    return true;
}

