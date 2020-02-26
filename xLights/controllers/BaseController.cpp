
/***************************************************************
 * This source files comes from the xLights project
 * https://www.xlights.org
 * https://github.com/smeighan/xLights
 * See the github commit history for a record of contributing
 * developers.
 * Copyright claimed based on commit dates recorded in Github
 * License: https://github.com/smeighan/xLights/blob/master/License.txt
 **************************************************************/

#include "BaseController.h"
#include "../outputs/OutputManager.h"
#include "../models/ModelManager.h"
#include "../outputs/ControllerEthernet.h"

#include <curl/curl.h>

#include <log4cpp/Category.hh>

#pragma region Constructors and Destructors
BaseController::BaseController(const std::string& ip, const std::string &proxy) : _ip(ip), _fppProxy(proxy), _baseUrl("") {

    static log4cpp::Category &logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    if (!_fppProxy.empty()) {
        _baseUrl = "/proxy/" + _ip;
    } 
}
#pragma endregion

#pragma region Protected Functions
std::string BaseController::GetURL(const std::string& url, bool logresult, const std::string& username, const std::string& password) {

    static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));
    std::string res;
    std::string const baseIP = _fppProxy.empty() ? _ip : _fppProxy;

    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, std::string("http://" + baseIP + _baseUrl + url).c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

        std::string response_string;

        if (username != "")
        {
            curl_easy_setopt(curl, CURLOPT_USERNAME, (const char*)username.c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, (const char*)password.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        /* Perform the request, res will get the return code */
        CURLcode r = curl_easy_perform(curl);

        if (r != CURLE_OK) {
            logger_base.error("Failure to access %s: %s.", (const char*)url.c_str(), curl_easy_strerror(r));
        }
        else {
            res = response_string;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return res;
}

std::string BaseController::PutURL(const std::string& url, const std::string& request, bool logresult, const std::string& username, const std::string& password) {

    static log4cpp::Category& logger_base = log4cpp::Category::getInstance(std::string("log_base"));

    std::string const baseIP = _fppProxy.empty() ? _ip : _fppProxy;
    logger_base.debug("Making request to Controller '%s'.", (const char*)url.c_str());
    logger_base.debug("    With data '%s'.", (const char*)request.c_str());

    CURL* curl = curl_easy_init();
    if (curl != nullptr) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, std::string("http://" + baseIP + _baseUrl + url).c_str());

        struct curl_slist* headers = NULL;

        headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.c_str());

        if (username != "")
        {
            curl_easy_setopt(curl, CURLOPT_USERNAME, (const char*)username.c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, (const char*)password.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
        std::string buffer = "";
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        CURLcode  ret = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (ret == CURLE_OK) {
            return buffer;
        }
        else {
            logger_base.error("Failure to access %s: %s.", (const char*)url.c_str(), curl_easy_strerror(ret));
        }
    }

    return "";
}
#pragma endregion