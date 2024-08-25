

// don't change those includes
#include "RSUser.h"
#include "RecommenderSystem.h"


// implement your cpp code here
RSUser::RSUser(const std::string &user_name, rank_map &rank_map1,
               std::shared_ptr<RecommenderSystem>& rec_sys_ptr) {
    user_name_ = user_name;
    rank_map_ = rank_map1;
    rec_sys_ptr_ = rec_sys_ptr;
}

const std::string& RSUser::get_name() const{
    return user_name_;
}

void RSUser::add_movie_to_rs(const std::string &name, int year,
                     const std::vector<double> &features,
                     double rate){
    auto movie_sp = rec_sys_ptr_->add_movie(name, year, features);
    rank_map_[movie_sp] = rate;
}

rank_map RSUser::get_ranks() const{
    return rank_map_;
}

sp_movie RSUser::get_recommendation_by_content() const{
    return rec_sys_ptr_->recommend_by_content(*this);
}

sp_movie RSUser::get_recommendation_by_cf(int k) const{
    return rec_sys_ptr_->recommend_by_cf(*this, k);
}

double RSUser::get_prediction_score_for_movie(const std::string& name,
                                                int year, int k) const{
    return rec_sys_ptr_->predict_movie_score(*this,
                                     rec_sys_ptr_->get_movie(name, year), k);
}

std::ostream& operator<<(std::ostream &os, const RSUser& rs_user){
    os << "name: " << rs_user.get_name() << std::endl;
    // use << of RecommenderSystem
    os << *rs_user.rec_sys_ptr_;
    return os;
}


