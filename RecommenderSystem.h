//
// Created on 2/20/2022.
//

#ifndef RECOMMENDERSYSTEM_H
#define RECOMMENDERSYSTEM_H
#include "RSUser.h"
#include <map>
#include <set>
#include <cmath>
#include <algorithm>


class RecommenderSystem
{
private:
    struct map_comparator{
        bool operator()(const std::pair<std::string, int>& pair1,
                        const std::pair<std::string, int>& pair2) const{
            return ((pair1.second < pair2.second) ||
            ((pair1.second == pair2.second) && pair1.first < pair2.first));
        }
    };
    // key == <name, year>                value == <sp_movie, features vector>
    std::map<std::pair<std::string, int>, std::pair<sp_movie,
            std::vector<double>>, map_comparator> map;
    // helpers declarations
    double get_rankings_sum(rank_map rank_map1);
    rank_map get_normalized_rankings(const rank_map& rank_map1);
    std::vector<double> get_vec_scalar_mult(double scalar,
                                        std::vector<double> features_vec);
    std::vector<double>& add_two_vec(std::vector<double>& v1,
                                     const std::vector<double>& v2);
    double inner_products(const std::vector<double>& v1,
                          const std::vector<double>& v2);
    double norm(const std::vector<double>& vec);
    std::set<sp_movie> get_k_movies(const sp_movie&
                movie_ptr, const std::set<sp_movie>& ranked_movies, int k,
                std::vector<std::pair<sp_movie, double>>& temp_Vec);
    double predicted_ranking(const RSUser& user,
                 const std::set<sp_movie>& k_set ,
                 const std::vector<std::pair<sp_movie,double>>& temp_Vec);

public:
	explicit RecommenderSystem() = default;
    /**
     * adds a new movie to the system
     * @param name name of movie
     * @param year year it was made
     * @param features features for movie
     * @return shared pointer for movie in system
     */
	sp_movie add_movie(const std::string& name,
                       int year,const std::vector<double>& features);


    /**
     * a function that calculates the movie with highest
     * score based on movie features
     * @param ranks user ranking to use for algorithm
     * @return shared pointer to movie in system
     */
	sp_movie recommend_by_content(const RSUser& user);

    /**
     * a function that calculates the movie with highest
     * predicted score based on ranking of other movies
     * @param ranks user ranking to use for algorithm
     * @param k
     * @return shared pointer to movie in system
     */
	sp_movie recommend_by_cf(const RSUser& user, int k);


    /**
     * Predict a user rating for a movie given argument
     * using item cf procedure with k most similar movies.
     * @param user_rankings: ranking to use
     * @param movie: movie to predict
     * @param k:
     * @return score based on algorithm as described in pdf
     */
	double predict_movie_score(const RSUser &user, const sp_movie &movie,
												  int k);

	/**
	 * gets a shared pointer to movie in system
	 * @param name name of movie
	 * @param year year movie was made
	 * @return shared pointer to movie in system
	 */
	sp_movie get_movie(const std::string &name, int year) const;


	friend std::ostream& operator<<(std::ostream &os,
            const RecommenderSystem& rhs);

};


#endif //RECOMMENDERSYSTEM_H
