#include "utils.hpp"

size_t writecallback(void* contents, size_t size, size_t nmemb, void* helper) {
    string* response = static_cast<string*>(helper);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}
