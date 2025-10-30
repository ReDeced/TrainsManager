#ifndef USERS_H
#define USERS_H

#include <iostream>
#include <vector>
#include <string>
#include "trains.h"

class User;

struct Ticket {
    int train_id;
    int car_id;
    std::string from_station;
    std::string to_station;
    double price;
    const User* owner;

    void print_ticket() const;

    void save_to_file() const;
};

struct Passport {
    std::string name;
    std::string surname;
    u_int64_t passport_id;
    std::string birth_date;
};

class User {
protected:
    int id;
    Passport passport;
    std::vector<Ticket> tickets;

public:
    User(int id, const Passport& passport) : id(id), passport(passport) {};

    void add_ticket(const Ticket& ticket);

    void buy_ticket(const Train& train, const std::string& from, const std::string& to, int car_id);

    void show_tickets() const;

    const Passport& get_passport() const { return passport; }

    int get_id() const { return id; }

    const std::vector<Ticket>& get_tickets() const { return tickets; }

    std::vector<Ticket>& get_tickets() { return tickets; }
};

#endif // USERS_H
