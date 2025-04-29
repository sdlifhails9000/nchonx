#include "http.h"

#include <string.h>

static HttpMethod str_to_method(const std::string& method) {
    if (method == "GET")
        return HttpMethod::GET;
    else if (method == "POST")
        return HttpMethod::POST;
    else if (method == "PATCH")
        return HttpMethod::PATCH;
    else if (method == "PUT")
        return HttpMethod::PUT;
    else if (method == "DELETE")
        return HttpMethod::DELETE;
    else if (method == "CONNECT")
        return HttpMethod::CONNECT;
    else if (method == "OPTIONS")
        return HttpMethod::OPTIONS;
    else if (method == "TRACE")
        return HttpMethod::TRACE;

    return HttpMethod::UNKNOWN_METHOD;
}

/* Technique stolen from nginx source (Specifically, from nginx_http_parse.c). */
int HttpRequest::parse_request_line(const std::string& line) {
    enum {
        s_start = 0,
        s_method,
        s_space_before_resource,
        s_resource,
        s_space_after_resource,
        s_h,
        s_ht,
        s_htt,
        s_http,
        s_major_version,
        s_after_major,
        s_minor_version,
        s_space_after_version,
        s_almost_done
    } state = s_start;

    std::string method;
    std::string resource;
    std::string version;
    
    for (const char& c : line) {
        switch (state) {
        case s_start:
            if (c > 'Z' || c < 'A')
                return HTTP_INVALID_REQUEST;

            state = s_method;
            break;

        case s_method:
            if (c == ' ') {
                state = s_space_before_resource;
                this->method = str_to_method(method);
                break;
            }

            if (c > 'Z' || c < 'A')
                return HTTP_INVALID_REQUEST;

            method.push_back(c);
            break;

        case s_space_before_resource:
            if (c != '/')
                return HTTP_INVALID_REQUEST;

            resource.push_back(c);
            state = s_resource;
            break;

        case s_resource:
            if (c == ' ') {
                state = s_space_after_resource;
                this->resource = resource;
                break;
            }

            resource.push_back(c);
            break;

        case s_space_after_resource:
            switch (c) {
            case 'H':
                state = s_h;
                break;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_h:
            switch (c) {
            case 'T':
                state = s_ht;
                break;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_ht:
            switch (c) {
            case 'T':
                state = s_htt;
                break;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_htt:
            switch (c) {
            case 'P':
                state = s_http;
                break;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_http:
            switch (c) {
            case '/':
                state = s_major_version;
                break;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_major_version:
            if (c > '9' || c < '0')
                return HTTP_INVALID_REQUEST;

            this->major = c - '0';
            state = s_after_major;
            break;
        case s_after_major:
            switch (c) {
            case ' ':
                state = s_space_after_version;
                break;
            case '\n':
                goto done;
            case '\r':
                state = s_almost_done;
                break;
            case '.':
                state = s_minor_version;
                break
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_minor_version:
            switch (c) {
            case ' ':
                state = s_space_after_version;
                break;
            case '\r':
                state = s_almost_done;
                break;
            case '\n':
                goto done;
            default:
                if (c > '9' || c < '0')
                    return HTTP_INVALID_REQUEST;
                this->major = c - '0';
            }
            break;
        
        case s_space_after_version:
            switch (c) {
            case ' ':
                break;
            case '\n':
                goto done;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;

        case s_almost_done:
            switch (c) {
            case '\n':
                goto done;
            default:
                return HTTP_INVALID_REQUEST;
            }
            break;
        }
    }

done:
    return HTTP_OK;
}
