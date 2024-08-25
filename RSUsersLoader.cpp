#include "RSUsersLoader.h"
#define SPACE_CH 32
#define MAX_FEATURE_RATE 10


std::vector<std::string> RSUsersLoader::split_string(
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


std::vector<RSUser> RSUsersLoader::create_users_from_file(const std::string&
users_file_path, std::unique_ptr<RecommenderSystem> rs) noexcept(false){
    std::vector<RSUser> result_vec;
    std::ifstream file;
    std::string all_lines, line;
    file.open(users_file_path); if (!file){ throw;}
    while(getline(file, line)){
        // removing '\r' character from line except for the last line
        if (line.at(line.length() - 1) == '\r'){
            line = line.substr(0, line.length() - 1);
        }
        all_lines += line;
        all_lines += '\n';
    }
    std::vector<std::string> lines_vector = split_string(all_lines,'\n');
    // initialize vector of movie-year
    std::vector<std::string> movies_vector =
            split_string(lines_vector[0], SPACE_CH);
    // initialize vector of pairs, each pair is <<movie name, year>, features>
    std::vector<std::pair<std::pair<std::string, int>,
            std::vector<double>>> movie_info_vector;
    movie_info_vector.reserve(movies_vector.size());
    for (int j = 0; (unsigned) j < movies_vector.size();j++){ //for each movie
        std::vector<std::string> movie = split_string (movies_vector[j], '-');
        std::string movie_name = movie[0];
        int movie_year = stoi(movie[1]);
        std::pair<std::string, int> pair = {movie_name, movie_year};
        // pointer of the movie in the system
        sp_movie movie_ptr = rs->get_movie(movie_name, movie_year);
        std::vector<double> movie_features = std::vector<double>();
        // add info to movies info vector
        movie_info_vector.push_back({pair,movie_features});
    }
    // rest of the vector is users and their rankings of each movie
    // converting unique pointer to shared pointer
    std::shared_ptr<RecommenderSystem> shared = std::move(rs);
    for(int j = 1; (unsigned) j < lines_vector.size(); j++){
        std::vector<std::string>user_vector=
                split_string(lines_vector[j], SPACE_CH);
        // user name is first string in the line
        std::string user_name = user_vector[0];
        rank_map user_rank_map(0, sp_movie_hash, sp_movie_equal);
        for(int k = 1; (unsigned) k < user_vector.size(); k++){
            std::string temp_movie_name = movie_info_vector[k-1].first.first;
            int temp_movie_year = movie_info_vector[k-1].first.second;
            std::vector<double> temp_movie_features =
                    movie_info_vector[k-1].second;
            double user_rate;
            // checking user's rate
            if (user_vector[k] == "NA"){
                user_rate = 0;
            }
            else {
                user_rate = stoi(user_vector[k]);
            }
            auto test = shared->get_movie(temp_movie_name,temp_movie_year);
            user_rank_map[shared->get_movie(temp_movie_name,temp_movie_year)] = user_rate;
        }
        RSUser new_user(user_name, user_rank_map, shared);
        result_vec.push_back(new_user);
    }
    return result_vec;
}
