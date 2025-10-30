#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <string>
#include "trains.h"
#include "users.h"

class Manager {
private:
    std::string filename;
    std::vector<Station> stations;
    std::vector<TrainType> train_types;
    std::vector<Route> routes;
    std::vector<Car> cars;
    std::vector<Train> trains;
    std::vector<User> users;

public:
    Manager();
    Manager(const std::string& filename);

    void load_from_file(const std::string& filename);
    void save_to_file(const std::string& filename) const;

    void load_from_file();
    void save_to_file() const;

    Ticket buy_ticket();

    User& register_user();

    User* get_user(u_int64_t passport_id);

    ~Manager();
};

#endif // MANAGER_H
