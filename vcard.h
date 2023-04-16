#ifndef VCARD_H
#define VCARD_H

#include <string>
#include <vector>

using namespace std;

/* Vcard version 4.0
 * Supported fields:
 * ADR: optional. A structured representation of the physical delivery address.
 * BDAY: optional. Date of birth of the individual.
 * EMAIL: optional.
 * FN: required. The formatted name string.
 * N: optional. A structured representation of the name of the person, place or
 * thing.
 * NOTE: optional. Specifies supplemental information or a comment.
 * ORG: optional. The name and optionally the unit(s) of the organization.
 * PHOTO: optional. An image or photograph of the individual.
 * TEL: optional. The canonical number string for a telephone number.
 * TITLE: optional. Specifies the job title, functional position or function of
 * the individual.
 * URL: optional. A URL pointing to a website that represents the person in some
 * way.
 * Technical required fields:
 * BEGIN: All vCards must start with this property.
 * END: All vCards must end with this property.
 * VERSION: The version of the vCard specification.
 */

class Vcard {

private:
    string addr;
    string bday;
    string email;
    string fn = "FN:";
    string n;
    string note;
    string org;
    string photo;
    string tel_home;
    string tel_work;
    string title;
    string url;
    const string BEGIN = "BEGIN:VCARD";
    const string END = "END:VCARD";
    const string VERSION = "VERSION:4.0";

    bool isDigit(string data);
    string splitName(string name);
    string clearSpaces(string data);
    string oneSpace(string data);
    bool checkPhone(string data);
    bool addImage(string image, string* save, string type);
    vector<unsigned char> readFile(string file_name);
    string base64_encode(vector<unsigned char> in);

public:
    Vcard();
    string createVCF();
    void addPerson(string prefix, string given_name, string middle_name,
                   string last_name, string suffix);
    bool addBday(string bdate);
    bool addAddress(string street_house, string city, string state, string zip,
                    string country);
    bool addHomePhone(string phone);
    bool addWorkPhone(string phone);
    bool addEmail(string mail);
    bool addPhoto(string picture);
    void addBussiness(string organization, string function);
    bool addUrl(string link);
    void addNote(string comment);
    bool saveVCF(string file_name);
};

#endif // VCARD_H
