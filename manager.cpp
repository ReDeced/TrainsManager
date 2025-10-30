#include "manager.h"
#include "users.h"
#include "trains.h"
#include "json/single_include/nlohmann/json.hpp"
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

void Manager::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Не удалось открыть JSON файл: " << filename << std::endl;
        return;
    }

    json data;
    file >> data;

    stations.clear();
    train_types.clear();
    cars.clear();
    routes.clear();
    trains.clear();
    users.clear();

    // --- Станции ---
    for (const auto& s : data["stations"]) {
        stations.push_back({ s["name"], { s["coords"][0], s["coords"][1] } });
    }

    // --- Типы поездов ---
    for (const auto& t : data["train_types"]) {
        train_types.push_back({ t["name"], t["price_coef"] });
    }

    // --- Вагоны ---
    for (const auto& c : data["cars"]) {
        cars.push_back({ c["id"], c["price_coef"], c["seats_count"] });
    }

    // --- Маршруты ---
    for (const auto& r : data["routes"]) {
        Route route;
        for (const auto& station_name : r["stations"]) {
            auto it = std::find_if(stations.begin(), stations.end(),
                                   [&](const Station& s) { return s.name == station_name; });
            if (it != stations.end()) route.add_station(*it);
        }
        routes.push_back(route);
    }

    // --- Поезда ---
    for (const auto& tr : data["trains"]) {
        int id = tr["id"];
        std::string train_type_name = tr["train_type"];
        std::string route_name = tr["route"];
        double base_price = tr["base_price"];

        auto type_it = std::find_if(train_types.begin(), train_types.end(),
                                    [&](const TrainType& t){ return t.name == train_type_name; });

        auto route_it = routes.begin();
        if (!routes.empty()) route_it = routes.begin();  // Для простоты

        if (type_it != train_types.end() && route_it != routes.end()) {
            Train train(id, *type_it, *route_it, base_price);
            for (const auto& d : tr["departure_dates"]) train.add_departure_date(d);
            for (const auto& c_id : tr["cars"]) {
                auto car_it = std::find_if(cars.begin(), cars.end(),
                                           [&](const Car& car){ return car.get_id() == c_id; });
                if (car_it != cars.end()) train.add_car(*car_it);
            }
            trains.push_back(train);
        }
    }

    // --- Пользователи и билеты ---
    if (data.contains("users")) {
        for (const auto& u : data["users"]) {
            Passport p;
            p.name = u["passport"]["name"];
            p.surname = u["passport"]["surname"];
            p.passport_id = u["passport"]["passport_id"];
            p.birth_date = u["passport"]["birth_date"];

            int user_id = p.passport_id;
            users.push_back(User(user_id, p));
        }

        for (size_t i = 0; i < data["users"].size(); ++i) {
            const auto& u = data["users"][i];
            User& user = users[i];

            if (u.contains("tickets")) {
                for (const auto& t : u["tickets"]) {
                    Ticket ticket;
                    ticket.train_id = t["train_id"];
                    ticket.car_id = t["car_id"];
                    ticket.from_station = t["from_station"];
                    ticket.to_station = t["to_station"];
                    ticket.price = t["price"];
                    ticket.owner = &user;

                    user.add_ticket(ticket);
                }
            }
        }
    }

    std::cout << "✅ Данные загружены из " << filename << std::endl;
}

void Manager::save_to_file(const std::string& filename) const {
    json data;

    // --- Станции ---
    for (const auto& s : stations) {
        data["stations"].push_back({ {"name", s.name}, {"coords", {s.position.x, s.position.y}} });
    }

    // --- Типы поездов ---
    for (const auto& t : train_types) {
        data["train_types"].push_back({ {"name", t.name}, {"price_coef", t.price_coef} });
    }

    // --- Вагоны ---
    for (const auto& c : cars) {
        data["cars"].push_back({ {"id", c.get_id()}, {"price_coef", c.get_price_coef()}, {"seats_count", c.get_seats_free()} });
    }

    // --- Маршруты ---
    for (const auto& r : routes) {
        json route_json;
        for (const auto& s : r.get_stations()) route_json.push_back(s.name);
        data["routes"].push_back({ {"stations", route_json} });
    }

    // --- Поезда ---
    for (const auto& tr : trains) {
        json train_json;
        train_json["id"] = tr.get_id();
        train_json["train_type"] = tr.get_type().name;
        train_json["route"] = tr.get_route().get_stations().front().name; // можно хранить маршрут как название первой станции
        train_json["base_price"] = tr.get_base_price();
        train_json["departure_dates"] = tr.get_departure_dates();

        json cars_json;
        for (const auto& c : tr.get_cars()) cars_json.push_back(c.get_id());
        train_json["cars"] = cars_json;

        data["trains"].push_back(train_json);
    }

    // --- Пользователи и билеты ---
    json users_json;
    for (const auto& u : users) {
        json user_json;
        user_json["passport"] = {
            {"name", u.get_passport().name},
            {"surname", u.get_passport().surname},
            {"passport_id", u.get_passport().passport_id},
            {"birth_date", u.get_passport().birth_date}
        };

        json tickets_json;
        for (const auto& t : u.get_tickets()) {
            tickets_json.push_back({
                {"train_id", t.train_id},
                {"car_id", t.car_id},
                {"from_station", t.from_station},
                {"to_station", t.to_station},
                {"price", t.price}
            });
        }
        user_json["tickets"] = tickets_json;

        users_json.push_back(user_json);
    }
    data["users"] = users_json;

    // --- Сохраняем файл ---
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Не удалось сохранить файл " << filename << std::endl;
        return;
    }
    file << std::setw(4) << data;
    std::cout << "✅ Данные сохранены в файл: " << filename << std::endl;
}

void Manager::load_from_file() {
    load_from_file(filename);
}

void Manager::save_to_file() const {
    save_to_file(filename);
}

Manager::Manager(const std::string& filename) : filename(filename) {
    load_from_file(filename);
}

Manager::~Manager() {
    save_to_file();
}


User& Manager::register_user() {
    Passport p;
    std::cout << "Введите имя: ";
    std::getline(std::cin, p.name);
    std::cout << "Введите фамилию: ";
    std::getline(std::cin, p.surname);
    std::cout << "Введите номер паспорта (10 цифр): ";
    std::cin >> p.passport_id;
    std::cin.ignore();
    std::cout << "Введите дату рождения (ГГГГ-ММ-ДД): ";
    std::getline(std::cin, p.birth_date);

    int new_id = users.empty() ? 1 : users.back().get_id() + 1;
    users.emplace_back(new_id, p);

    std::cout << "Пользователь зарегистрирован! ID: " << new_id << "\n";
    return users.back();
}

User* Manager::get_user(int passport_id) {
    for (auto& u : users) {
        if (u.get_passport().passport_id == passport_id) {
            std::cout << "Добро пожаловать, " << u.get_passport().name << "!\n";
            return &u;
        }
    }
    std::cout << "Пользователь с таким паспортом не найден.\n";
    return nullptr;
}
