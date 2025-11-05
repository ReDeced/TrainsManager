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
    std::vector<Train> trains;
    std::vector<User> users;

public:
    Manager();
    Manager(const std::string& filename);

    void load_from_file(const std::string& filename);
    void save_to_file(const std::string& filename) const;

    void load_from_file();
    void save_to_file() const;

    void show_routes() const;

    Ticket& buy_ticket(User& user, int train_id, int car_id, const std::string& from, const std::string& to);

    User& register_user();

    User* get_user(u_int64_t passport_id);

    ~Manager();
};

#endif // MANAGER_H
