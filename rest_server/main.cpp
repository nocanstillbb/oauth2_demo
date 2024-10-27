#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <jwt-cpp/jwt.h>
#include <iostream>
#include <map>

using namespace web;
using namespace http;
using namespace http::experimental::listener;

const std::string SECRET_KEY = "your_secret_key"; // Same as above

bool is_token_valid(const std::string& token, std::string& role) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{ SECRET_KEY })
            .with_issuer("auth_server");

        verifier.verify(decoded);
        auto role_claim = decoded.get_payload_claim("role"); // Use get_payload_claim
        role = role_claim.as_string(); // Extract role from token
        return true;
    } catch (...) {
        return false;
    }
}

void handle_stateful_request(http_request request) {
    auto authorization_header = request.headers().find(U("Authorization"));
    if (authorization_header == request.headers().end()) {
        request.reply(status_codes::Unauthorized, U("Missing Authorization header"));
        return;
    }
    
    auto token = authorization_header->second.substr(7); // Remove "Bearer "
    std::string role;

    if (!is_token_valid(token, role)) {
        request.reply(status_codes::Unauthorized, U("Invalid token"));
        return;
    }

    // Role-based access control (RBAC)
    json::value response_data;
    if (role == "admin") {
        response_data[U("message")] = json::value::string(U("Welcome, admin!"));
    } else {
        response_data[U("message")] = json::value::string(U("Welcome, user!"));
    }
    
    request.reply(status_codes::OK, response_data);
}

int main() {
    uri_builder uri(U("http://localhost:8081/service"));
    auto addr = uri.to_uri().to_string();
    http_listener l(addr);

    l.support(methods::GET, handle_stateful_request);
    l
        .open()
        .then([&l](){ std::cout << "Starting to listen at: " << l.uri().to_string() << std::endl; })
        .wait();

    std::string line;
    std::getline(std::cin, line);
    return 0;
}
