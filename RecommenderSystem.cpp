#include "RecommenderSystem.h"

// helpers

double RecommenderSystem::get_rankings_sum(rank_map rank_map1){
    //! sum rankings of the user
    double vector_sum = 0;
    // calculate vector_sum of vector
    for (auto it = rank_map1.begin(); it != rank_map1.end(); it++){
        vector_sum += it->second;
    }
    return vector_sum;
}


rank_map RecommenderSystem::get_normalized_rankings
                        (const rank_map& rank_map1){
    //! get normalized rankings of the user
    rank_map normalized_rank_map = rank_map1;
    // calculate vector_sum of vector
    double vector_sum = get_rankings_sum(rank_map1);
    // decrement the vector coordinates by the vector's vector_sum
    for (auto it = normalized_rank_map.begin();
            it != normalized_rank_map.end(); it++){
        if (it->second > 0){
            it->second -= vector_sum;
        }
    }
    return normalized_rank_map;
}


std::vector<double> RecommenderSystem::get_vec_scalar_mult
                (double scalar, std::vector<double> features_vec){
    //! multiply the given vector by the given scalar and return a new vector
    std::vector<double> res = features_vec;
    for (auto it = res.begin(); it != res.end(); it++){
        *it *= scalar;
    }
    return res;
}


std::vector<double>& RecommenderSystem::add_two_vec(std::vector<double>& v1,
                                            const std::vector<double>& v2){
    //! adds the two vectors together
    auto it1 = v1.begin();
    auto it2 = v2.begin();
    while (it1 != v1.end()){  // same as it2 != v2.end()
        *it1 += *it2;
        it1++;
        it2++;
    }
    return v1;
}


double RecommenderSystem::inner_products(const std::vector<double>& v1,
                                         const std::vector<double>& v2){
    //! calculates the inner products of the given vectors
    double res = 0;
    auto it1 = v1.begin();
    auto it2 = v2.begin();
    // assuming both of the same size
    while (it1 != v1.end()){  // same as it2 != v2.end()
        res += (*it1 * *it2);
        it1++;
        it2++;
    }
    return res;
}


double RecommenderSystem::norm(const std::vector<double>& vec){
    //! calculates the norm of the given vector
    double res = 0;
    for (auto it = vec.begin(); it != vec.end(); it++){
        res += std::pow(*it, 2);
    }
    return std::sqrt(res);
}


bool vector_comparator(const std::pair<sp_movie, double>& pair1,
                const std::pair<sp_movie, double>& pair2){
    return pair1.second > pair2.second;
}


std::set<sp_movie> RecommenderSystem::get_k_movies(const sp_movie& movie_ptr,
                               const std::set<sp_movie>& ranked_movies,
                int k, std::vector<std::pair<sp_movie, double>>& temp_vec) {
//! gets the k-most similar movies to unranked movie pointed to by movie_ptr
    std::set<sp_movie> res;
    std::string movie_name = movie_ptr->get_name();
    int year = movie_ptr->get_year();
    std::pair<std::string, int> pair = {movie_name, year};
    std::vector<double> features_vec = map[pair].second;
    // go over the movies
    for (auto it = ranked_movies.begin(); it != ranked_movies.end(); it++) {
        std::string current_movie_name = (*it)->get_name();
        int current_year = (*it)->get_year();
        std::pair<std::string, int> current_pair =
                {current_movie_name, current_year};
        std::vector<double> other_features_vec = map[current_pair].second;
        // calculate the similarity
        double current_similarity =
                inner_products(features_vec, other_features_vec) /
                            (norm(features_vec) * norm(other_features_vec));
        // add to the vector
        std::pair<sp_movie, double> current_pair1 = {*it, current_similarity};
        temp_vec.push_back(current_pair1);
    }
    std::sort(temp_vec.begin(), temp_vec.end(), vector_comparator);
    // get first k (most similar) items
    auto it = temp_vec.begin();
    int i = 0;
    while (i < k){
        res.insert(it->first);
        it++;
        i++;
    }
    return res;
}


double RecommenderSystem::predicted_ranking(const RSUser& user,
    const std::set<sp_movie>& k_set , const std::vector<std::pair<sp_movie,
                        double>>& temp_Vec){
    //! get prediction of ranking of *movie_ptr based on k_set
    rank_map user_ranks = user.get_ranks();
    double nominator = 0, denominator = 0;
    for (auto it = k_set.begin(); it != k_set.end(); it++) {
        double rank = user_ranks[*it];
        double similarity;
        for (auto it1 = temp_Vec.begin(); it1 != temp_Vec.end(); it1++){
            if (it1->first == *it){  // get movie's similarity
                similarity = it1->second;
            }
        }
        nominator += (similarity * rank);
        denominator += similarity;
    }
    return nominator / denominator;
}


// implementations

sp_movie RecommenderSystem::add_movie(const std::string& name,
                              int year,const std::vector<double>& features){
    std::pair<std::string, int> key;
    std::pair<sp_movie, std::vector<double>> value;
    key.first = name, key.second = year;
    value.first = std::make_shared<Movie>(name, year);
    value.second = features;
    map[key] = value;
    return value.first;
}


sp_movie RecommenderSystem::recommend_by_content(const RSUser& user){
    // step 1 - get normalized rankings
    rank_map normalized_rankings = get_normalized_rankings(user.get_ranks());
    // step 2 - get preferences of user
    std::vector<double> preferences_vec;
    bool flag = true;
    for (auto it = normalized_rankings.begin();
            it != normalized_rankings.end(); it++){
        // if user has ranked the current movie
        if (it->second != 0){
            double scalar = it->second;
            std::string movie_name = it->first->get_name();
            int year = it->first->get_year();
            std::pair<std::string, int> pair = {movie_name, year};
            std::vector<double> mult_vec = get_vec_scalar_mult
                            (scalar, map[pair].second);
            if (flag){
                preferences_vec.resize(map[pair].second.size());
                flag=false;
            }
            preferences_vec = add_two_vec(preferences_vec, mult_vec);
        }
    }
    // step 3 - get most similar movie
    sp_movie res_ptr;
    double similarity = -1;
    for (auto it = normalized_rankings.begin();
            it != normalized_rankings.end(); it++){
        // if user has NOT ranked the current movie
        if (it->second == 0){
            std::string movie_name = it->first->get_name();
            int year = it->first->get_year();
            std::pair<std::string, int> pair = {movie_name, year};
            std::vector<double> features_vec = map[pair].second;
            // calculate similarity
            double current_similarity =
                    inner_products(preferences_vec, features_vec) /
                        (norm(preferences_vec) * norm(features_vec));
            // get the sp_movie according to the max similarity
            if (current_similarity > similarity){
                similarity = current_similarity;
                res_ptr = map[pair].first;
            }
        }
    }
    return res_ptr;
}


sp_movie RecommenderSystem::recommend_by_cf(const RSUser& user, int k){
    sp_movie res;
    // step 1 - find set of k most similar movies
    rank_map user_ranks = user.get_ranks();
    // temp vector to hold pairs of sp_movie and its similarity to *movie_ptr
    std::vector<std::pair<sp_movie, double>> temp_vec;
    // variable to hold max predicted rank
    double max_prediction = 0;
    // get set of ranked movies
    std::set<sp_movie> ranked_movies_set;
    for (auto it = user_ranks.begin(); it != user_ranks.end(); it++){
        // if user has ranked the current movie
        if (it->second > 0){
            ranked_movies_set.insert(it->first);
        }
    }
    // step 2 - predict movies ranking and return
    // pointer to the max predicted movie
    for (auto it = user_ranks.begin(); it != user_ranks.end(); it++){
        // if user has NOT ranked the current movie
        if (it->second == 0){
            std::set<sp_movie> k_set = get_k_movies(
                        it->first, ranked_movies_set, k, temp_vec);
            double prediction = predicted_ranking(user, k_set, temp_vec);
            if (prediction > max_prediction){
                max_prediction = prediction;
                res = it->first;
            }
        }
    }
    return res;
}


double RecommenderSystem::predict_movie_score(const RSUser &user,
                                              const sp_movie &movie, int k){
    sp_movie res;
    // step 1 - find set of k most similar movies
    rank_map user_ranks = user.get_ranks();
    // temp vector to hold pairs of sp_movie and its similarity to *movie_ptr
    std::vector<std::pair<sp_movie, double>> temp_vec;
    // get set of ranked movies
    std::set<sp_movie> ranked_movies_set;
    for (auto it = user_ranks.begin(); it != user_ranks.end(); it++){
        // if user has ranked the current movie
        if (it->second > 0){
            ranked_movies_set.insert(it->first);
        }
    }
    // step 2 - predict movie's ranking
    std::set<sp_movie> k_set = get_k_movies(movie,
                                        ranked_movies_set, k, temp_vec);
    double prediction = predicted_ranking(user, k_set, temp_vec);
    return prediction;
}


sp_movie RecommenderSystem::get_movie(const std::string &name, int year)const{
    std::pair<std::string, int> pair = {name, year};
    return map.at(pair).first;
}

std::ostream& operator<<(std::ostream &os, const RecommenderSystem& rhs){
    for (auto it = rhs.map.begin(); it != rhs.map.end(); it++) {
        os << *it->second.first;  // print the movie class
    }
    return os;
}

