#include "RecommenderSystemLoader.h"
#define SPACE_CH 32
#define MAX_FEATURE_RATE 10


std::vector<std::string> RecommenderSystemLoader::split_string(
        const std::string& str, char splitter) {
    std::vector<std::string> result;
    std::string current;
    for (int i = 0; (unsigned) i < str.size(); i++) {
        if (str[i] == splitter) {
            if (!current.empty()) {
                result.push_back(current);
                current = "";
            }
            continue;
        }
        current += str[i];
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}


std::unique_ptr<RecommenderSystem>
        RecommenderSystemLoader::create_rs_from_movies_file
        (const std::string &movies_file_path) noexcept (false){
    std::unique_ptr<RecommenderSystem> rs_ptr =
            std::make_unique<RecommenderSystem>();
    std::ifstream file(movies_file_path);
    if (!file){
        throw;
    }
    std::string all_lines, line;
    while(getline(file, line)){
        // removing '\r' character from line except for the last line
        if (line.at(line.length() - 1) == '\r'){
            line = line.substr(0, line.length() - 1);
        }
        all_lines += line;
        all_lines += '\n';
    }
    std::vector<std::string> lines_vector = split_string
            (all_lines, '\n');
    for(int i = 0; (unsigned) i < lines_vector.size(); i++){
        std::vector<std::string> temp2 = split_string (lines_vector[i], '-');
        // first element is movie's name
        std::string movie_name = temp2[0];
        // second element is year and preferences values
        std::string numbers = temp2[1];
        // splitting numbers by spaces (32 is ascii value of space character)
        std::vector<std::string> temp3 = split_string(numbers, SPACE_CH);
        // first number is the year
        int year = stoi(temp3[0]);
        // declaring features vector
        std::vector<double> features;
        features.reserve(temp3.size() - 1);
        // going over the string from the second element,
        // adding to the features vector
        for (int j = 1; (unsigned) j < temp3.size(); j++){
            double n = stod(temp3[j]);
            if ((n >= 1 && n <= MAX_FEATURE_RATE) && (std::floor(n) == n)){
                features.push_back(n);
            }
            else{ // deallocating the vectors
                temp3.clear();
                temp2.clear();
                lines_vector.clear();
                throw std::out_of_range("invalid score");
            }
        }
        rs_ptr->add_movie(movie_name, year, features);
    }
    return rs_ptr;
}

