#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "email.h"


// Callback function for libcurl to read the email payload
static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
{
    std::vector<std::string>* payload_text = static_cast<std::vector<std::string>*>(userp);
    const char* data;
    size_t len;

    if (size == 0 || nmemb == 0 || payload_text->empty()) {
        return 0;
    }



    data = payload_text->front().c_str();
    len = payload_text->front().length();

    // Copy data to the buffer
    if (len > size * nmemb)
        len = size * nmemb;
    memcpy(ptr, data, len);

    // Remove the sent line
    payload_text->erase(payload_text->begin());

    return len;
}

void send(const Config* config, const std::string recipient, const std::string subject, const std::string body) {
    CURL* curl;
    CURLcode res = CURLE_OK;

    // SMTP server details
    const std::string smtp_server = "smtp.gmail.com";
    const int smtp_port = 587;
    const std::string username = config->projectEmail;
    const std::string password = config->projectEmailPassword;

    // Email details
    const std::string mail_from = config->projectEmail;

    // Construct the email headers and body
    std::vector<std::string> email_payload;
    email_payload.push_back("Date: " + std::string(__DATE__) + " " + std::string(__TIME__) + "\r\n");
    email_payload.push_back("To: " + recipient + "\r\n");
    email_payload.push_back("From: " + mail_from + "\r\n");
    email_payload.push_back("Subject: " + subject + "\r\n");

    email_payload.push_back("MIME-Version: 1.0\r\n");
    email_payload.push_back("Content-Type: text/html; charset=utf-8\r\n");

    email_payload.push_back("\r\n"); // Empty line to separate headers from body
    email_payload.push_back(body + "\r\n");
    email_payload.push_back(".\r\n"); // SMTP end of data marker

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

        std::string url = "smtp://" + smtp_server + ":" + std::to_string(smtp_port);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mail_from.c_str());

        // Add recipient
        struct curl_slist* recipients = NULL;
        recipients = curl_slist_append(recipients, recipient.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Set the payload for the email
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &email_payload);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L); // Indicate an upload (sending email)


        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error("Error while sending an email");
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
    else {
        throw std::runtime_error("Error while sending an email");
    }

}