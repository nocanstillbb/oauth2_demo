#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <jwt-cpp/jwt.h>
#include <iostream>
#include <map>
#include <chrono>

using namespace web;
using namespace http;
using namespace http::experimental::listener;

const std::string SECRET_KEY = "your_secret_key"; // Change this
const int TOKEN_EXPIRATION_MINUTES = 15;
const int REFRESH_TOKEN_EXPIRATION_DAYS = 7;

std::map<std::string, std::pair<std::string, std::string>> users = {
    {"user", {"password", "user_role"}}, // Hardcoded user credentials and roles
};

std::string generate_token(const std::string& username, const std::string& role) {
    auto token = jwt::create()
        .set_issuer("auth_server")
        .set_subject(username)
        .set_payload_claim("role", jwt::claim(role))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes(TOKEN_EXPIRATION_MINUTES))
        .sign(jwt::algorithm::hs256{ SECRET_KEY });

    return token;
}

std::string generate_refresh_token(const std::string& username) {
    auto expiration_time = std::chrono::system_clock::now() + std::chrono::hours(24 * REFRESH_TOKEN_EXPIRATION_DAYS);
    auto refresh_token = jwt::create()
        .set_issuer("auth_server")
        .set_subject(username)
        .set_expires_at(expiration_time)
        .sign(jwt::algorithm::hs256{ SECRET_KEY });

    return refresh_token;
}

void handle_login(http_request request) {
    request.extract_json().then([=](json::value request_data) {
        auto username = request_data[U("username")].as_string();
        auto password = request_data[U("password")].as_string();

        if (users.find(username) != users.end() && users[username].first == password) {
            json::value response_data;
            auto role = users[username].second;
            response_data[U("access_token")] = json::value::string(generate_token(username, role));
            response_data[U("refresh_token")] = json::value::string(generate_refresh_token(username));
            response_data[U("token_type")] = json::value::string(U("Bearer"));
            response_data[U("expires_in")] = json::value::number(TOKEN_EXPIRATION_MINUTES * 60);

            request.reply(status_codes::OK, response_data);
        } else {
            request.reply(status_codes::Unauthorized, U("Invalid credentials"));
        }
    }).wait();
}

void handle_refresh(http_request request) {
    request.extract_json().then([=](json::value request_data) {
        auto refresh_token = request_data[U("refresh_token")].as_string();
        try {
            auto decoded = jwt::decode(refresh_token);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ SECRET_KEY })
                .with_issuer("auth_server");

            verifier.verify(decoded);

            auto username = decoded.get_subject();
            //auto role = decoded.get_payload_claim("role").as_string();
            auto role = users[username].second;
            json::value response_data;
            response_data[U("access_token")] = json::value::string(generate_token(username, role));
            response_data[U("expires_in")] = json::value::number(TOKEN_EXPIRATION_MINUTES * 60);
            request.reply(status_codes::OK, response_data);
        } catch (...) {
            request.reply(status_codes::Unauthorized, U("Invalid refresh token"));
        }
    }).wait();
}




int main() {
    uri_builder uri(U("http://localhost:8080"));
    auto addr = uri.to_uri().to_string();
    http_listener listener(addr);

    listener.support(methods::POST, [](http_request request) { 
        if (request.request_uri().path() == U("/login")) {
            handle_login(request);
        } else if (request.request_uri().path() == U("/refresh")) {
            handle_refresh(request);
        } else {
            request.reply(status_codes::NotFound, U("Endpoint not found"));
        }
    });

    listener
        .open()
        .then([&listener]() {
            std::cout << "Starting to listen at: " << listener.uri().to_string() << std::endl;
        })
        .wait();

    std::string line;
    std::getline(std::cin, line);
    return 0;
}

