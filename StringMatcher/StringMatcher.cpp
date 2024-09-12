#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

std::string format_with_commas(long long number) {
    std::ostringstream oss;
    oss.imbue(std::locale(""));
    oss << number;
    return oss.str();
}

std::string format_large_number(double number) {
    std::ostringstream oss;
    if (number >= 1'000'000'000'000'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000'000'000'000'000.0) << "Sp"; // septillion, anything past this is just wild
    }
    else if (number >= 1'000'000'000'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000'000'000'000.0) << "Sx"; // sextillion
    }
    else if (number >= 1'000'000'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000'000'000.0) << "Qn"; // quintillion
    }
    else if (number >= 1'000'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000'000.0) << "T"; // trillion
    }
    else if (number >= 1'000'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000'000.0) << "M"; // million
    }
    else if (number >= 1'000) {
        oss << std::fixed << std::setprecision(3) << (number / 1'000.0) << "K"; // thousand
    }
    else {
        oss << format_with_commas(static_cast<long long>(number));
    }
    return oss.str();
}

std::string format_odds(double odds) {
    std::ostringstream oss;
    if (odds < 1) {
        oss << "1 in " << format_large_number(static_cast<long long>(1 / odds));
    }
    else {
        oss << "1 in " << format_large_number(static_cast<long long>(odds));
    }
    return oss.str();
}

std::string generate_random_string(std::mt19937& gen, std::uniform_int_distribution<>& dist, const std::string& chars, int length) {
    std::string result(length, '\0');
    std::generate(result.begin(), result.end(), [&]() { return chars[dist(gen)]; });
    return result;
}

std::string determine_character_set(const std::string& target) {
    std::string char_set;
    if (std::any_of(target.begin(), target.end(), ::isalpha)) {
        char_set += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    }
    if (std::any_of(target.begin(), target.end(), ::isdigit)) {
        char_set += "0123456789";
    }
    if (std::any_of(target.begin(), target.end(), [](char c) { return !std::isspace(c) && !std::isalnum(c); })) {
        char_set += "!\"#$%&'()*+,-./:;<=>?@[\\]^_{|}~";
    }
    return char_set;
}

std::string format_duration(double seconds) {
    int years = static_cast<int>(seconds / 31'536'000);
    seconds -= years * 31'536'000;
    int days = static_cast<int>(seconds / 86'400);
    seconds -= days * 86'400;
    int hours = static_cast<int>(seconds / 3'600);
    seconds -= hours * 3'600;
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

std::string remove_spaces(const std::string& str) {
    std::string result;
    std::copy_if(str.begin(), str.end(), std::back_inserter(result), [](char c) { return !std::isspace(c); });
    return result;
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

    target_input = remove_spaces(target_input);

    std::string char_set = determine_character_set(target_input);
    if (char_set.empty()) {
        std::cerr << "Target string does not contain any recognizable characters." << std::endl;
        return 1;
    }

    int string_length = target_input.length();
    double total_combinations = std::pow(char_set.size(), string_length);
    double probability_of_success = 1.0 / total_combinations;

    std::cout << "Odds of finding the string in 1 attempt: " << format_odds(total_combinations) << "\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, char_set.size() - 1);

    long long generated_attempts = 0;

    std::cout << "Attempting to match string \"" << target_input << "\"\n";

    auto start_time = std::chrono::high_resolution_clock::now();

    bool found_match = false;

    while (!found_match) {
        std::string random_str = generate_random_string(gen, dist, char_set, string_length);
        generated_attempts++;

        if (random_str == target_input) {
            std::cout << "Match found: " << random_str << " at attempt " << format_large_number(generated_attempts) << std::endl;
            found_match = true;
        }

        if (generated_attempts % 1'000'000 == 0) {
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = current_time - start_time;
            std::cout << "Generated " << format_large_number(generated_attempts) << " attempts in " << format_duration(elapsed.count()) << ".\n";
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
