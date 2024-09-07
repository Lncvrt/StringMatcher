#include <iostream>
#include <random>
#include <chrono>
#include <unordered_set>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <iomanip>

std::string generate_random_string(std::mt19937& gen, std::uniform_int_distribution<>& dist, const std::string& chars, int length) {
    std::string result(length, '\0');
    for (int i = 0; i < length; ++i) {
        int rand_val = dist(gen);
        result[i] = chars[rand_val];
    }
    return result;
}

std::string determine_character_set(const std::string& target) {
    bool has_letters = false;
    bool has_digits = false;
    bool has_symbols = false;

    for (char c : target) {
        if (std::isalpha(c)) {
            has_letters = true;
        }
        else if (std::isdigit(c)) {
            has_digits = true;
        }
        else {
            has_symbols = true;
        }
    }

    std::string char_set;
    if (has_letters) {
        char_set += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    }
    if (has_digits) {
        char_set += "0123456789";
    }
    if (has_symbols) {
        char_set += "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
    }

    return char_set;
}

std::string format_duration(double seconds) {
    int years = static_cast<int>(seconds / 31536000);
    seconds -= years * 31536000;
    int days = static_cast<int>(seconds / 86400);
    seconds -= days * 86400;
    int hours = static_cast<int>(seconds / 3600);
    seconds -= hours * 3600;
    int minutes = static_cast<int>(seconds / 60);
    seconds -= minutes * 60;
    int secs = static_cast<int>(seconds);

    std::ostringstream oss;
    if (years > 0) oss << years << "y ";
    if (days > 0 || years > 0) oss << days << "d ";
    if (hours > 0 || days > 0 || years > 0) oss << hours << "h ";
    if (minutes > 0 || hours > 0 || days > 0 || years > 0) oss << minutes << "m ";
    oss << secs << "s";

    return oss.str();
}

std::string format_large_number(long long number) {
    std::ostringstream oss;
    if (number >= 1'000'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000'000.0) << "B";
    }
    else if (number >= 1'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000.0) << "M";
    }
    else if (number >= 1'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000.0) << "K";
    }
    else {
        oss << number;
    }

    return oss.str();
}

int main(int argc, char* argv[]) {
    std::string target_input;

    if (argc < 2) {
        std::cout << "Enter target to match: ";
        std::getline(std::cin, target_input);
    }
    else {
        target_input = argv[1];
    }

    std::istringstream stream(target_input);
    std::string word;
    std::unordered_set<std::string> target_words;
    while (stream >> word) {
        target_words.insert(word);
    }

    std::string char_set = determine_character_set(target_input);
    if (char_set.empty()) {
        std::cerr << "Target string does not contain any recognizable characters." << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, char_set.size() - 1);

    long long generated_attempts = 0;

    std::cout << "Attempting to match string \"" << target_input << "\"\n";

    auto start_time = std::chrono::high_resolution_clock::now();

    bool found_match = false;

    while (!found_match) {
        for (const std::string& target_word : target_words) {
            int string_length = target_word.length();
            std::string random_str = generate_random_string(gen, dist, char_set, string_length);
            generated_attempts++;

            if (random_str == target_word) {
                std::cout << "Match found: " << random_str << " at attempt " << format_large_number(generated_attempts) << std::endl;
                found_match = true;
                break;
            }

            if (generated_attempts % 1'000'000 == 0) {
                auto current_time = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = current_time - start_time;
                std::cout << "Generated " << format_large_number(generated_attempts) << " attempts in " << format_duration(elapsed.count()) << ".\n";
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Generated " << format_large_number(generated_attempts) << " attempts in " << format_duration(elapsed.count()) << ".\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Rate: " << format_large_number(static_cast<long long>(generated_attempts / elapsed.count())) << " attempts per second.\n";

    std::cout << "Press Enter to exit...\n";
    std::cin.get();

    return 0;
}
